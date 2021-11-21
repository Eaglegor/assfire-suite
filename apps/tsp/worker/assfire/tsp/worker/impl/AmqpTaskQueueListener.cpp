#include "AmqpTaskQueueListener.hpp"
#include "assfire/api/v1/tsp/worker.pb.h"
#include "TspWorkerConstants.hpp"

namespace assfire::tsp {
    AmqpTaskQueueListener::AmqpTaskQueueListener(std::unique_ptr<RabbitMqConnector> rabbit_mq_connector)
            : rabbit_mq_connector(std::move(rabbit_mq_connector)) {}

    void AmqpTaskQueueListener::startListening() {
        listener = std::make_unique<RabbitMqConnector::Listener>(rabbit_mq_connector->listen(
                TSP_WORKER_AMQP_TASK_QUEUE_NAME,
                TSP_WORKER_AMQP_TASK_EXCHANGE,
                TSP_WORKER_AMQP_TASK_CHANNEL
        ));
    }

    std::string AmqpTaskQueueListener::nextTask() {
        std::string result;
        listener->next([&](const amqp_envelope_t &envelope) {
            api::v1::tsp::WorkerTask worker_task;
            worker_task.ParseFromArray(envelope.message.body.bytes, envelope.message.body.len);
            result = worker_task.task_id();
        });
        return result;
    }
}