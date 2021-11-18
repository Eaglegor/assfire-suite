#pragma once

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <memory>
#include <string>
#include "assfire/tsp/worker/TaskProcessor.hpp"
#include "assfire/tsp/worker/SolutionPublisher.hpp"
#include "assfire/tsp/worker/SavedStateManager.hpp"
#include "RabbitMqControlChannelListener.hpp"
#include "RabbitMqConnector.hpp"

namespace assfire::tsp {
    class TspSolverEngine;
}

namespace assfire::tsp {

    class RabbitMqTaskProcessor : public TaskProcessor {
    public:
        RabbitMqTaskProcessor(std::unique_ptr<TspSolverEngine> tsp_solver, std::unique_ptr<SolutionPublisher> solution_publisher, std::unique_ptr<SavedStateManager> saved_state_manager,
                              const std::string &host, int port, const std::string &login, const std::string &password);

        [[noreturn]]
        void startProcessing() override;

    private:
        std::unique_ptr<TspSolverEngine> tsp_solver;
        std::unique_ptr<SolutionPublisher> solution_publisher;
        std::unique_ptr<SavedStateManager> saved_state_manager;
        util::RabbitMqConnector task_rabbit_mq_connector;
        util::RabbitMqConnector status_rabbit_mq_connector;
        RabbitMqControlChannelListener control_channel_listener;
    };
}