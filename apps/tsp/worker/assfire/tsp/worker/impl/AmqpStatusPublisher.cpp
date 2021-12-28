#include "AmqpStatusPublisher.hpp"
#include <assfire/api/v1/tsp/worker.pb.h>
#include "assfire/api/v1/tsp/translators/TspCostTranslator.hpp"
#include "assfire/api/v1/tsp/translators/TspValidationResultTranslator.hpp"
#include "TspWorkerConstants.hpp"
#include "TspWorkerKeys.hpp"
#include "assfire/util/amqp/AmqpProtoEncode.hpp"

namespace assfire::tsp
{
    using WorkerStatusUpdate = assfire::api::v1::tsp::WorkerTspStatusUpdate;

    AmqpStatusPublisher::AmqpStatusPublisher(std::string name, util::AmqpConnectionPool &connection_pool)
            : name(std::move(name)), connection_pool(connection_pool) {}

    namespace
    {
        void publish(util::AmqpConnectionPool::PublisherRef &publisher, const WorkerStatusUpdate &update) {
            try {
                publisher->publish<WorkerStatusUpdate>(update, util::AmqpProtoEncode<WorkerStatusUpdate>());
            } catch (const util::amqp_exception &e) {
                SPDLOG_ERROR("Failed to publish status {} to AMQP server for task {}: {}", update.type(), update.task_id(), e.what());
            }
        }
    }

    void AmqpStatusPublisher::publishStarted(const std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_STARTED);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
    }

    void AmqpStatusPublisher::publishPaused(const std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_PAUSED);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
        releasePublisher(task_id);
    }

    void AmqpStatusPublisher::publishInterrupted(const std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
        releasePublisher(task_id);
    }

    void AmqpStatusPublisher::publishError(const std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_ERROR);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
        releasePublisher(task_id);
    }

    void AmqpStatusPublisher::publishFinished(const std::string &task_id) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED);
        update.set_task_id(task_id);
        publish(getPublisher(task_id), update);
        releasePublisher(task_id);
    }

    void AmqpStatusPublisher::publishNewSolution(const std::string &task_id, const TspCost &cost, const TspValidationResult &validation_result) {
        assfire::api::v1::tsp::WorkerTspStatusUpdate update;
        update.set_type(api::v1::tsp::WorkerTspStatusUpdate_Type_WORKER_TSP_STATUS_UPDATE_TYPE_NEW_SOLUTION);
        update.set_task_id(task_id);
        update.mutable_cost()->CopyFrom(api::v1::tsp::TspCostTranslator::toProto(cost));
        update.mutable_validation_result()->CopyFrom(api::v1::tsp::TspValidationResultTranslator::toProto(validation_result));
        publish(getPublisher(task_id), update);
    }

    util::AmqpConnectionPool::PublisherRef &AmqpStatusPublisher::getPublisher(const std::string &task_id) {
        {
            std::lock_guard<std::mutex> lock(publishers_lock);
            auto iter = publishers.find(task_id);
            if (iter != publishers.end()) return iter->second;
        }

        auto publisher =
                connection_pool.createPublisher(name + "/" + task_id, {
                        {
                                TSP_WORKER_AMQP_STATUS_EXCHANGE,
                                "",
                                false,
                                false
                        }
                });

        {
            std::lock_guard<std::mutex> lock(publishers_lock);
            publishers[task_id] = std::move(publisher);
        }

        return publishers[task_id];
    }

    void AmqpStatusPublisher::releasePublisher(const std::string &task_id) {
        std::lock_guard<std::mutex> lock(publishers_lock);
        publishers.erase(task_id);
    }

    void AmqpStatusPublisher::release(const std::string &task_id) {
        releasePublisher(task_id);
    }
}