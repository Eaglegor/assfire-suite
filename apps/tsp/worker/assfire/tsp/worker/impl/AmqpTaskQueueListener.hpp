#pragma once

#include <memory>
#include "RabbitMqConnector.hpp"
#include "assfire/tsp/worker/TaskProvider.hpp"
#include "assfire/tsp/worker/TaskQueueListener.hpp"

namespace assfire::tsp {
    class AmqpTaskQueueListener : public TaskQueueListener {
    public:
        explicit AmqpTaskQueueListener(std::unique_ptr<RabbitMqConnector> rabbit_mq_connector);

        void startListening() override;

        std::string nextTask() override;

    private:
        std::unique_ptr<RabbitMqConnector> rabbit_mq_connector;
        std::unique_ptr<RabbitMqConnector::Listener> listener;
    };
}