#pragma once

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <memory>
#include <string>
#include "TaskProcessor.hpp"
#include "SolutionPublisher.hpp"
#include "SavedStateManager.hpp"
#include "RabbitMqControlChannelListener.hpp"
#include "RabbitMqConnector.hpp"

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

        [[noreturn]]
        void startProcessing() override;

    private:
        std::unique_ptr<TspSolverEngine> tsp_solver;
        std::unique_ptr<SolutionPublisher> solution_publisher;
        std::unique_ptr<SavedStateManager> saved_state_manager;
        RabbitMqConnector task_rabbit_mq_connector;
        RabbitMqConnector status_rabbit_mq_connector;
        RabbitMqControlChannelListener control_channel_listener;
    };
}