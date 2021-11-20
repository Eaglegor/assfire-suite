#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "RabbitMqConnector.hpp"
#include "assfire/tsp/worker/StatusPublisher.hpp"

namespace assfire::tsp {
    class AmqpStatusPublisher : public StatusPublisher {
    public:
        AmqpStatusPublisher(std::unique_ptr<RabbitMqConnector> rabbit_mq_connector);

        void publishStarted(std::string &task_id) override;

        void publishPaused(std::string &task_id) override;

        void publishInterrupted(std::string &task_id) override;

        void publishError(std::string &task_id) override;

        void publishFinished(std::string &task_id) override;

        void publishNewSolution(std::string &task_id, const TspCost &cost, const TspValidationResult &validation_result) override;

    private:
        std::mutex publishers_lock;
        RabbitMqConnector::Publisher getPublisher(const std::string &task_id);
        void releasePublisher(const std::string &task_id);

        std::unique_ptr<RabbitMqConnector> rabbit_mq_connector;
        std::unordered_map<std::string, RabbitMqConnector::Publisher> publishers;
    };
}