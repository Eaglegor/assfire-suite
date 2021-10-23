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

    void processAmqpError(const amqp_rpc_reply_t &reply, const char *message, bool throw_error = true) {
        switch (reply.reply_type) {
            case AMQP_RESPONSE_NONE: {
                SPDLOG_ERROR("{}: {}", message, "Missing AMQP reply type");
                break;
            }

            case AMQP_RESPONSE_LIBRARY_EXCEPTION: {
                SPDLOG_ERROR("{}: {}", message, amqp_error_string2(reply.library_error));
                break;
            }

            case AMQP_RESPONSE_SERVER_EXCEPTION: {
                switch (reply.reply.id) {
                    case AMQP_CONNECTION_CLOSE_METHOD: {
                        amqp_connection_close_t *m =
                                (amqp_connection_close_t *) reply.reply.decoded;
                        SPDLOG_ERROR("{}: error {} ({})", message, m->reply_code, (const char *) m->reply_text.bytes);
                        break;
                    }
                    case AMQP_CHANNEL_CLOSE_METHOD: {
                        amqp_channel_close_t *m = (amqp_channel_close_t *) reply.reply.decoded;
                        SPDLOG_ERROR("{}: error {} ({})", message, m->reply_code, (const char *) m->reply_text.bytes);
                        break;
                    }
                    default:
                        amqp_channel_close_t *m = (amqp_channel_close_t *) reply.reply.decoded;
                        SPDLOG_ERROR("{}: unknown AMQP server error", message);
                }
                break;
            }
        }
        if (throw_error) throw std::runtime_error(message);
    }

    RabbitMqTaskProcessor::RabbitMqTaskProcessor(std::unique_ptr<TspSolverEngine> tsp_solver, std::unique_ptr<SolutionPublisher> solution_publisher,
                                                 std::unique_ptr<SavedStateManager> saved_state_manager,
                                                 const std::string &host, int port, const std::string &login, const std::string &password)
            : tsp_solver(std::move(tsp_solver)), solution_publisher(std::move(solution_publisher)) {
        SPDLOG_INFO("Initializing RabbitMQ task processor");

        SPDLOG_INFO("Creating RabbitMQ socket");
        connection = amqp_new_connection();
        socket = amqp_tcp_socket_new(connection);

        try {
            if (!socket) {
                SPDLOG_ERROR("Failed to create RabbitMQ socket");
                throw std::runtime_error("Failed to create RabbitMQ socket");
            }

            SPDLOG_INFO("Connecting to RabbitMQ at {}:{}", host, port);
            int status = amqp_socket_open(socket, host.c_str(), port);
            if (status != AMQP_STATUS_OK) {
                SPDLOG_ERROR("Failed to connect to RabbitMQ: {}", amqp_error_string2(status));
                throw std::runtime_error("Failed to connect to RabbitMQ");
            }

            SPDLOG_INFO("Logging in to RabbitMQ as {}...", login.c_str());
            amqp_rpc_reply_t reply = amqp_login(connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, login.c_str(), password.c_str());

            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                processAmqpError(reply, "Failed to log in to RabbitMQ");
            }

            SPDLOG_INFO("Opening RabbitMQ channel #{}...", AMQP_CHANNEL_ID);
            amqp_channel_open(connection, AMQP_CHANNEL_ID);
            reply = amqp_get_rpc_reply(connection);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                processAmqpError(reply, "Failed to open RabbitMQ channel");
            }
        } catch (const std::runtime_error &e) {
            SPDLOG_ERROR("Failed to initialize RabbitMQ transport. Cleaning up connections...");
            amqp_connection_close(connection, AMQP_REPLY_SUCCESS);
            amqp_destroy_connection(connection);
            SPDLOG_ERROR("RabbitMQ cleanup finished");
            throw e;
        }

        SPDLOG_INFO("RabbitMQ task processor successfully initialized");
    }

    RabbitMqTaskProcessor::~RabbitMqTaskProcessor() {
        SPDLOG_INFO("Destroying RabbitMQ task processor");

        SPDLOG_INFO("Closing RabbitMQ channel #{}...", AMQP_CHANNEL_ID);
        amqp_rpc_reply_t reply = amqp_channel_close(connection, AMQP_CHANNEL_ID, AMQP_REPLY_SUCCESS);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to close RabbitMQ channel", false);
        }

        SPDLOG_INFO("Closing RabbitMQ connection...");
        reply = amqp_connection_close(connection, AMQP_REPLY_SUCCESS);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to close RabbitMQ connection", false);
        }

        SPDLOG_INFO("Destroying RabbitMQ connection...");
        int status = amqp_destroy_connection(connection);
        if (status != AMQP_STATUS_OK) {
            processAmqpError(reply, "Failed to destroy RabbitMQ connection", false);
        }

        SPDLOG_INFO("RabbitMQ task processor destroyed");
    }

    void RabbitMqTaskProcessor::startProcessing() {
        SPDLOG_INFO("Subscribing for RabbitMQ tasks queue {}", TASK_QUEUE_NAME);
        amqp_queue_declare(connection, AMQP_CHANNEL_ID, amqp_cstring_bytes(TASK_QUEUE_NAME.c_str()),
                           0,0,0,1,amqp_empty_table);
        amqp_rpc_reply_t_ reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to declare RabbitMQ queue");
        }

        amqp_queue_bind(connection, AMQP_CHANNEL_ID, amqp_cstring_bytes(TASK_QUEUE_NAME.c_str()), amqp_cstring_bytes(TASK_EXCHANGE.c_str()),
                        amqp_empty_bytes, amqp_empty_table);
        reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to bind RabbitMQ queue");
        }

        amqp_basic_consume(connection, AMQP_CHANNEL_ID, amqp_cstring_bytes(TASK_QUEUE_NAME.c_str()),
                           amqp_empty_bytes, 0, 0, 0, amqp_empty_table);
        reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to subscribe to RabbitMQ queue");
        }

        SPDLOG_INFO("Starting consuming RabbitMQ tasks");
        for (;;) {
            amqp_envelope_t_ envelope;
            amqp_maybe_release_buffers(connection);

            SPDLOG_DEBUG("Waiting for next message...");
            reply = amqp_consume_message(connection, &envelope, NULL, 0);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                processAmqpError(reply, "There was an error while retrieving message from RabbitMQ queue");
            }
            SPDLOG_DEBUG("Received {} bytes from queue {}", envelope.message.body.bytes, TASK_QUEUE_NAME);

            assfire::api::v1::tsp::WorkerTask worker_task;
            worker_task.ParseFromArray(envelope.message.body.bytes, envelope.message.body.len);
            SPDLOG_INFO("Received new task from RabbitMQ queue. Task id: {}", worker_task.task_id());

            TspAlgorithmStateContainer saved_state_container([&](const TspAlgorithmStateContainer::TspAlgorithmStateDto &state) {
                saved_state_manager->saveState(worker_task.task_id(), state);
            });
            std::optional<SavedStateManager::State> saved_state = saved_state_manager->loadState(worker_task.task_id());
            if (saved_state) {
                SPDLOG_INFO("Found saved state for task {}. Will use it as an initial state", worker_task.task_id());
                saved_state_container.setState(std::move(*saved_state));
            }

            SPDLOG_INFO("Starting tsp solution session for task {}", worker_task.task_id());
            TspSolutionSession session = tsp_solver->solveTsp(assfire::api::v1::tsp::TspTaskTranslator::fromProto(worker_task.task()), saved_state_container, [&](const TspSolution &solution) {
                SPDLOG_DEBUG("Got new solution for task {}, current_cost: {}", worker_task.task_id(), solution.getCost().getValue());
                solution_publisher->publish(worker_task.task_id(), solution);
            });

            SPDLOG_INFO("Started session {} for task {}", session.getId(), worker_task.task_id());

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
            SPDLOG_INFO("Finished processing for task {}", worker_task.task_id());
        }
    }
}