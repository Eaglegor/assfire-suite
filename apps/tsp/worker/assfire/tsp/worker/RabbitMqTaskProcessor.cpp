#include "RabbitMqTaskProcessor.hpp"
#include <assfire/tsp/engine/TspSolverEngine.hpp>
#include <spdlog/spdlog.h>
#include <assfire/api/v1/tsp/worker.pb.h>
#include <assfire/tsp/api/TspSolution.hpp>
#include <assfire/tsp/api/TspTask.hpp>
#include <assfire/api/v1/tsp/translators/TspTaskTranslator.hpp>
#include <chrono>

namespace assfire::tsp::worker {

    const std::string RabbitMqTaskProcessor::TASK_QUEUE_NAME("org.assfire.tsp.worker.task");
    const std::string RabbitMqTaskProcessor::CONTROL_QUEUE_NAME("org.assfire.tsp.worker.signal");
    const std::string RabbitMqTaskProcessor::RESULT_QUEUE_NAME("org.assfire.tsp.worker.result");

    namespace {
        constexpr int AMQP_CHANNEL_ID = 1;

        const std::string TASK_EXCHANGE = "amq.direct";
        const std::string SIGNAL_EXCHANGE = "amq.topic";
    }


    RabbitMqTaskProcessor::RabbitMqTaskProcessor(std::unique_ptr<TspSolverEngine> tsp_solver, std::unique_ptr<SolutionPublisher> solution_publisher,
                                                 std::unique_ptr<SavedStateManager> saved_state_manager,
                                                 const std::string &host, int port, const std::string &login, const std::string &password)
            : tsp_solver(std::move(tsp_solver)), solution_publisher(std::move(solution_publisher)), saved_state_manager(std::move(saved_state_manager)),
              task_rabbit_mq_connector("TaskProcessor"),
              status_rabbit_mq_connector("TaskProcessor::StatusUpdates"),
              control_channel_listener(host, port, login, password) {
        SPDLOG_INFO("Initializing RabbitMQ task processor");

        task_rabbit_mq_connector.connect(host, port, login, password);
        status_rabbit_mq_connector.connect(host, port, login, password);
        control_channel_listener.startListening();

        SPDLOG_INFO("RabbitMQ task processor successfully initialized");
    }

    void RabbitMqTaskProcessor::startProcessing() {
        SPDLOG_INFO("Starting RabbitMQ tasks processing");

        RabbitMqConnector::Publisher status_publisher = status_rabbit_mq_connector.publish("org.assfire.tsp.worker.status", "amq.topic", 3);
        task_rabbit_mq_connector.listen(TASK_QUEUE_NAME, TASK_EXCHANGE, AMQP_CHANNEL_ID, [&](const amqp_envelope_t_ &envelope) {
            assfire::api::v1::tsp::WorkerTask worker_task;
            worker_task.ParseFromArray(envelope.message.body.bytes, envelope.message.body.len);
            SPDLOG_INFO("Received new task from RabbitMQ queue. Task id: {}", worker_task.task_id());

            TspAlgorithmStateContainer saved_state_container([&](const TspAlgorithmStateContainer::TspAlgorithmStateDto &state) {
                saved_state_manager->saveState(worker_task.task_id(), state);
            });
            SPDLOG_INFO("Looking for saved state for task {}", worker_task.task_id());
            std::optional<SavedStateManager::State> saved_state = saved_state_manager->loadState(worker_task.task_id());
            if (saved_state) {
                SPDLOG_INFO("Found saved state for task {}. Will use it as an initial state", worker_task.task_id());
                saved_state_container.setState(std::move(*saved_state));
            }

            TspTask task = assfire::api::v1::tsp::TspTaskTranslator::fromProto(worker_task.task());
            TspSolutionListener solution_listener;
            solution_listener.setOnNextSolutionCallback([&](const TspSolution &solution) {
                SPDLOG_DEBUG("Got new solution for task {}, current_cost: {}", worker_task.task_id(), solution.getCost().getValue());
                solution_publisher->publish(worker_task.task_id(), task, solution);

                assfire::api::v1::tsp::WorkerTspStatusUpdate status_update;
                status_update.set_task_id(worker_task.task_id());
                void *msg = malloc(status_update.ByteSizeLong());
                status_update.SerializeToArray(msg, status_update.ByteSizeLong());
                status_publisher.publish(msg, status_update.ByteSizeLong());
                free(msg);
            }, true);
            solution_listener.setOnErrorCallback([&] {
                SPDLOG_ERROR("Error while calculating solution for task {}", worker_task.task_id());
                solution_publisher->onError(worker_task.task_id());

                assfire::api::v1::tsp::WorkerTspStatusUpdate status_update;
                status_update.set_task_id(worker_task.task_id());
                void *msg = malloc(status_update.ByteSizeLong());
                status_update.SerializeToArray(msg, status_update.ByteSizeLong());
                status_publisher.publish(msg, status_update.ByteSizeLong());
                free(msg);
            });

            SPDLOG_INFO("Starting tsp solution session for task {}", worker_task.task_id());

            TspSolutionSession session = tsp_solver->solveTsp(task, saved_state_container, solution_listener);
            SPDLOG_INFO("Started session {} for task {}", session.getId(), worker_task.task_id());

            SPDLOG_INFO("Subscribing for control signals for task {}", worker_task.task_id());
            control_channel_listener.setListeners(
                    [&](const std::string &task) {
                        if (task == worker_task.task_id()) {
                            SPDLOG_INFO("Received PAUSE signal for task {}", worker_task.task_id());
                            session.pause();
                        }
                    },
                    [&](const std::string &task) {
                        if (task == worker_task.task_id()) {
                            SPDLOG_INFO("Received INTERRUPT signal for task {}", worker_task.task_id());
                            session.interrupt();
                        }
                    }
            );

            auto start = std::chrono::system_clock::now();
            bool is_first = true;
            SPDLOG_INFO("Waiting for final TSP solution...");
            while (!session.isFinished()) {
                if (!is_first) {
                    auto now = std::chrono::system_clock::now();
                    SPDLOG_INFO("Still waiting for task {}, currently elapsed time: {}", worker_task.task_id(), std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count());
                }
                session.waitFor(5000);
                is_first = false;
            }

            SPDLOG_INFO("Unsubscribing from control signals for task {}", worker_task.task_id());
            control_channel_listener.resetListeners();

            SPDLOG_INFO("Finished processing for task {}", worker_task.task_id());
        });
    }
}