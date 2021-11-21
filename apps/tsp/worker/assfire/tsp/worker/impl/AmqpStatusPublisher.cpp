#include "AmqpStatusPublisher.hpp"
#include <assfire/api/v1/tsp/worker.pb.h>
#include "assfire/api/v1/tsp/translators/TspCostTranslator.hpp"
#include "assfire/api/v1/tsp/translators/TspValidationResultTranslator.hpp"
#include "TspWorkerConstants.hpp"
#include "TspWorkerKeys.hpp"

namespace assfire::tsp {

    AmqpStatusPublisher::AmqpStatusPublisher(std::unique_ptr<RabbitMqConnector> rabbit_mq_connector)
            : rabbit_mq_connector(std::move(rabbit_mq_connector)) {}

    namespace {
        void publish(RabbitMqConnector::Publisher publisher, const assfire::api::v1::tsp::WorkerTspStatusUpdate &update) {
            void *msg = malloc(update.ByteSizeLong());
            update.SerializeToArray(msg, update.ByteSizeLong());
            publisher.publish(msg, update.ByteSizeLong());
            free(msg);
        }
    }

    void AmqpStatusPublisher::publishStarted(std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_STARTED);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
    }

    void AmqpStatusPublisher::publishPaused(std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_PAUSED);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
        releasePublisher(task_id);
    }

    void AmqpStatusPublisher::publishInterrupted(std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
        releasePublisher(task_id);
    }

    void AmqpStatusPublisher::publishError(std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_ERROR);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
        releasePublisher(task_id);
    }

    void AmqpStatusPublisher::publishFinished(std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
        releasePublisher(task_id);
    }

    void AmqpStatusPublisher::publishNewSolution(std::string &task_id, const TspCost &cost, const TspValidationResult &validation_result) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_NEW_SOLUTION);
        update.set_task_id(task_id);
        update.mutable_cost()->CopyFrom(api::v1::tsp::TspCostTranslator::toProto(cost));
        update.mutable_validation_result()->CopyFrom(api::v1::tsp::TspValidationResultTranslator::toProto(validation_result));
        publish(getPublisher(task_id), update);
    }

    RabbitMqConnector::Publisher AmqpStatusPublisher::getPublisher(const std::string &task_id) {
        {
            std::lock_guard<std::mutex> lock(publishers_lock);
            auto iter = publishers.find(task_id);
            if (iter != publishers.end()) return iter->second;
        }

        RabbitMqConnector::Publisher publisher = rabbit_mq_connector->publish(
                statusQueueName(task_id),
                TSP_WORKER_AMQP_STATUS_EXCHANGE,
                TSP_WORKER_AMQP_STATUS_CHANNEL,
                true
        );

        {
            std::lock_guard<std::mutex> lock(publishers_lock);
            publishers.emplace(task_id, publisher);
        }

        return publisher;
    }

    void AmqpStatusPublisher::releasePublisher(const std::string &task_id) {
        std::lock_guard<std::mutex> lock(publishers_lock);
        publishers.erase(task_id);
    }
}