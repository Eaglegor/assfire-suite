#pragma once

#include "assfire/util/amqp/AmqpConnectionPool.hpp"
#include "assfire/tsp/worker/impl/TspWorkerConstants.hpp"

namespace assfire::tsp
{
    void declareTopology(util::AmqpConnectionPool &connection_pool) {
        // Declare exchanges
        connection_pool.declareExchange(TSP_WORKER_AMQP_TASK_EXCHANGE, {
                TSP_WORKER_AMQP_TASK_EXCHANGE,
                util::AmqpExchangeType::Direct,
                false,
                true,
                false,
                false
        });

        connection_pool.declareExchange(TSP_WORKER_AMQP_INTERRUPT_EXCHANGE, {
                TSP_WORKER_AMQP_INTERRUPT_EXCHANGE,
                util::AmqpExchangeType::Fanout,
                false,
                true,
                false,
                false
        });

        connection_pool.declareExchange(TSP_WORKER_AMQP_STATUS_EXCHANGE, {
                TSP_WORKER_AMQP_STATUS_EXCHANGE,
                util::AmqpExchangeType::Fanout,
                false,
                false,
                false,
                false
        });

        // Declare static queues
        connection_pool.declareQueue(TSP_WORKER_AMQP_TASK_QUEUE_NAME, {
                TSP_WORKER_AMQP_TASK_QUEUE_NAME,
                false,
                true,
                false,
                false
        });

        // Bind static queues
        connection_pool.bindQueue({
                                          TSP_WORKER_AMQP_TASK_QUEUE_NAME,
                                          TSP_WORKER_AMQP_TASK_EXCHANGE,
                                          TSP_WORKER_AMQP_TASK_QUEUE_NAME
                                  });
    }
}