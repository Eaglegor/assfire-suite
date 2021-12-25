#pragma once

#include <memory>
#include "RabbitMqConnector.hpp"
#include "assfire/tsp/worker/TaskProvider.hpp"
#include "assfire/tsp/worker/TaskQueueListener.hpp"
#include "assfire/util/amqp/AmqpConnectionPool.hpp"
#include <assfire/util/amqp/AmqpConsumer.hpp>

namespace assfire::tsp
{
    class AmqpTaskQueueListener : public TaskQueueListener
    {
    public:
        AmqpTaskQueueListener(std::string name, util::AmqpConnectionPool &connection_pool);

        void startListening() override;

        std::string nextTask() override;

    private:
        std::string name;
        util::AmqpConnectionPool &connection_pool;
        util::AmqpConnectionPool::ConsumerRef consumer;
    };
}