#pragma once

#include <memory>
#include "TaskQueueListener.hpp"
#include "InterruptListener.hpp"
#include "TaskProvider.hpp"
#include "StatusPublisher.hpp"
#include "SavedStateManager.hpp"
#include "SolutionPublisher.hpp"
#include "assfire/tsp/engine/TspSolverEngine.hpp"

namespace assfire::tsp {
    class Worker {
    public:
        Worker(std::unique_ptr<TaskQueueListener> task_queue_listener,
               std::unique_ptr<InterruptListener> interrupt_listener,
               std::unique_ptr<TaskProvider> task_provider,
               std::unique_ptr<StatusPublisher> status_publisher,
               std::unique_ptr<SavedStateManager> saved_state_manager,
               std::unique_ptr<SolutionPublisher> solution_publisher,
               std::unique_ptr<TspSolverEngine> engine);

        [[noreturn]]
        void start();

    private:
        std::unique_ptr<TaskQueueListener> task_queue_listener;
        std::unique_ptr<InterruptListener> interrupt_listener;
        std::unique_ptr<TaskProvider> task_provider;
        std::unique_ptr<StatusPublisher> status_publisher;
        std::unique_ptr<SavedStateManager> saved_state_manager;
        std::unique_ptr<SolutionPublisher> solution_publisher;
        std::unique_ptr<TspSolverEngine> engine;
    };
}