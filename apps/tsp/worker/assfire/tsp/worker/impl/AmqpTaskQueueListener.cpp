#include "AmqpTaskQueueListener.hpp"

#include <utility>
#include "assfire/api/v1/tsp/worker.pb.h"
#include "TspWorkerConstants.hpp"
#include "assfire/util/amqp/AmqpProtoParse.hpp"

namespace assfire::tsp
{
    using WorkerTask = api::v1::tsp::WorkerTask;

    AmqpTaskQueueListener::AmqpTaskQueueListener(std::string name,
                                                 util::AmqpConnectionPool &connection_pool)
            : name(std::move(name)), connection_pool(connection_pool) {}

    void AmqpTaskQueueListener::startListening() {
        consumer = connection_pool.createConsumer(name, {
                {
                        TSP_WORKER_AMQP_TASK_QUEUE_NAME,
                        false,
                        false,
                        false
                }
        });
    }

    void AmqpTaskQueueListener::nextTask(NewTaskProcessor process) {
        consumer->consumeMessage([&](util::AmqpDelivery &delivery) {
            try {
                process(delivery.parse<WorkerTask>(util::AmqpProtoParse<WorkerTask>()).task_id());
                delivery.ack();
            } catch (const std::exception &e) {
                delivery.nack(true);
            }
        });
    }
}