#include "Worker.hpp"
#include <spdlog/spdlog.h>
#include "assfire/tsp/engine/TspAlgorithmStateContainer.hpp"
#include "assfire/tsp/api/TspSolutionListener.hpp"
#include "assfire/tsp/engine/TspSolverEngine.hpp"
#include "impl/TspWorkerConstants.hpp"
#include <condition_variable>
#include <atomic>
#include <chrono>

namespace assfire::tsp {
    Worker::Worker(std::unique_ptr<TaskQueueListener> task_queue_listener,
                   std::unique_ptr<InterruptListener> interrupt_listener,
                   std::unique_ptr<TaskProvider> task_provider,
                   std::unique_ptr<StatusPublisher> status_publisher,
                   std::unique_ptr<SavedStateManager> saved_state_manager,
                   std::unique_ptr<SolutionPublisher> solution_publisher,
                   std::unique_ptr<TspSolverEngine> engine) :
            task_queue_listener(std::move(task_queue_listener)),
            interrupt_listener(std::move(interrupt_listener)),
            task_provider(std::move(task_provider)),
            status_publisher(std::move(status_publisher)),
            saved_state_manager(std::move(saved_state_manager)),
            solution_publisher(std::move(solution_publisher)),
            engine(std::move(engine)) {}

    void Worker::start() {
        SPDLOG_INFO("Starting TSP worker...");

        std::mutex cv_mutex;
        std::unique_lock<std::mutex> cv_lock(cv_mutex);

        task_queue_listener->startListening();
        interrupt_listener->startListening();

        while (true) {
            std::atomic_bool done = false;
            SPDLOG_INFO("Waiting for the next task...");
            std::string task_id = task_queue_listener->nextTask();

            SPDLOG_INFO("Received request to process TSP task {}", task_id);

            if (!task_provider->tryLock(task_id)) {
                SPDLOG_INFO("Couldn't acquire lock on proposed task {}. Likely someone else it already working on it", task_id);
                continue;
            }


            try {
                if (task_provider->isFinished(task_id)) {
                    SPDLOG_INFO("Task {} is already finished", task_id);
                    task_provider->unlock(task_id);
                    continue;
                }

                if (!task_provider->isPaused(task_id)) {
                    int attempts = task_provider->incAttempts(task_id);
                    if (attempts >= TSP_WORKER_MAX_ATTEMPTS) {
                        SPDLOG_INFO("Max attempts reached for task {}: {} >= {}", task_id, attempts, TSP_WORKER_MAX_ATTEMPTS);
                        task_provider->unlock(task_id);
                        continue;
                    }
                }

                SPDLOG_DEBUG("Trying to retrieve task {} from storage", task_id);
                std::optional<TspTask> task = task_provider->retrieveTask(task_id);
                if (!task) throw std::runtime_error("Failed to retrieve task from storage");

                SPDLOG_DEBUG("Trying to retrieve saved state for task {}", task_id);
                TspAlgorithmStateContainer saved_state_container([&](const TspAlgorithmStateContainer::TspAlgorithmStateDto &state) {
                    saved_state_manager->saveState(task_id, state);
                });
                std::optional<SavedStateManager::State> saved_state = saved_state_manager->loadState(task_id);
                if (saved_state) {
                    SPDLOG_INFO("Found saved state for task {}", task_id);
                    saved_state_container.setState(std::move(*saved_state));
                }

                std::condition_variable cv;

                SPDLOG_DEBUG("Creating solution listener for task {}", task_id);
                TspSolutionListener solution_listener;
                solution_listener.setOnNextSolutionCallback([&](const TspSolution &solution) {
                    SPDLOG_DEBUG("Got new solution for task {}, current_cost: {}", task_id, solution.getCost().getValue());
                    solution_publisher->publish(task_id, *task, solution);
                    status_publisher->publishNewSolution(task_id, solution.getCost(), solution.getValidationResult());

                    if (solution.isFinalSolution()) {
                        SPDLOG_INFO("Got final solution for task {}, cost: {}", task_id, solution.getCost().getValue());
                        status_publisher->publishFinished(task_id);
                        task_provider->sendFinished(task_id);
                        done = true;
                        cv.notify_all();
                    }
                }, true);

                solution_listener.setOnErrorCallback([&] {
                    SPDLOG_ERROR("Error while calculating solution for task {}", task_id);
                    status_publisher->publishError(task_id);
                    task_provider->sendError(task_id);
                    done = true;
                    cv.notify_all();
                });

                SPDLOG_DEBUG("Starting TSP session for task {}", task_id);
                status_publisher->publishStarted(task_id);
                task_provider->sendStarted(task_id);

                TspSolutionSession session = engine->solveTsp(task_id, *task, saved_state_container, solution_listener);

                interrupt_listener->subscribe(task_id, [&](int signal) {
                    switch (signal) {
                        case InterruptListener::PAUSE:
                            SPDLOG_INFO("Got PAUSE signal for task {}", task_id);
                            session.pause();
                            status_publisher->publishPaused(task_id);
                            task_provider->sendPaused(task_id);
                            done = true;
                            cv.notify_all();
                            break;
                        case InterruptListener::INTERRUPT:
                            SPDLOG_INFO("Got INTERRUPT signal for task {}", task_id);
                            session.interrupt();
                            status_publisher->publishInterrupted(task_id);
                            task_provider->sendPaused(task_id);
                            saved_state_manager->clearState(task_id);
                            done = true;
                            cv.notify_all();
                            break;
                        default:
                            SPDLOG_WARN("Got unknown signal {} for task {} - ignoring", signal, task_id);
                            break;
                    }
                });

                SPDLOG_INFO("Waiting for final TSP solution for task {}...", task_id);
                while (!cv.wait_until(cv_lock, std::chrono::system_clock::now() + std::chrono::seconds(5), [&] { return done.load(); })) {
                    SPDLOG_DEBUG("Solution is in progress for task {}...", task_id);
                    task_provider->sendInProgress(task_id);
                }

            } catch (const std::exception &e) {
                SPDLOG_INFO("There was an error while processing task {}: {}", task_id, e.what());
                status_publisher->publishError(task_id);
                task_provider->sendError(task_id);
            }

            SPDLOG_INFO("Finished processing TSP task {}", task_id);

            status_publisher->release(task_id);
            interrupt_listener->unsubscribe(task_id);
            task_provider->unlock(task_id);
        }
    }
}