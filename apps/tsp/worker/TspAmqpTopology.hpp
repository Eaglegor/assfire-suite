#pragma once

#include <chrono>
#include <thread>
#include "assfire/util/amqp/AmqpConnectionPool.hpp"
#include "assfire/tsp/worker/impl/TspWorkerConstants.hpp"

namespace assfire::tsp
{
    void declareTopology(util::AmqpConnectionPool &connection_pool) {
        while (true) {
            try {
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
                return;
            } catch (const util::amqp_exception &e) {
                SPDLOG_ERROR("Couldn't declare topology: {}. Will retry in 10s", e.what());
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
        }
    }
}