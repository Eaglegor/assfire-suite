#pragma once

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <memory>
#include <string>
#include "TaskProcessor.hpp"
#include "SolutionPublisher.hpp"
#include "SavedStateManager.hpp"

namespace assfire::tsp {
    class TspSolverEngine;
}

namespace assfire::tsp::worker {

    class RabbitMqTaskProcessor : public TaskProcessor {
    public:
        static const std::string TASK_QUEUE_NAME;
        static const std::string CONTROL_QUEUE_NAME;
        static const std::string RESULT_QUEUE_NAME;

        RabbitMqTaskProcessor(std::unique_ptr<TspSolverEngine> tsp_solver, std::unique_ptr<SolutionPublisher> solution_publisher, std::unique_ptr<SavedStateManager> saved_state_manager,
                              const std::string &host, int port, const std::string &login, const std::string &password);

        ~RabbitMqTaskProcessor();

        [[noreturn]]
        void startProcessing() override;

    private:
        std::unique_ptr<TspSolverEngine> tsp_solver;
        std::unique_ptr<SolutionPublisher> solution_publisher;
        std::unique_ptr<SavedStateManager> saved_state_manager;
        amqp_socket_t *socket = nullptr;
        amqp_connection_state_t connection;
    };
}