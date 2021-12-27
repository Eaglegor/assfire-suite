#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "RabbitMqConnector.hpp"
#include "assfire/tsp/worker/StatusPublisher.hpp"
#include "assfire/util/amqp/AmqpConnectionPool.hpp"

namespace assfire::tsp {
    class AmqpStatusPublisher : public StatusPublisher {
    public:
        AmqpStatusPublisher(std::string name, util::AmqpConnectionPool& connection_pool);

        void release(const std::string &task_id) override;

        void publishStarted(const std::string &task_id) override;

        void publishPaused(const std::string &task_id) override;

        void publishInterrupted(const std::string &task_id) override;

        void publishError(const std::string &task_id) override;

        void publishFinished(const std::string &task_id) override;

        void publishNewSolution(const std::string &task_id, const TspCost &cost, const TspValidationResult &validation_result) override;

    private:
        std::string name;
        util::AmqpConnectionPool& connection_pool;
        std::mutex publishers_lock;
        util::AmqpConnectionPool::PublisherRef& getPublisher(const std::string &task_id);
        void releasePublisher(const std::string &task_id);

        std::unordered_map<std::string, util::AmqpConnectionPool::PublisherRef> publishers;
    };
}