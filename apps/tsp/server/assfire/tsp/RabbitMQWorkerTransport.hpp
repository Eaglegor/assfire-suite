#pragma once

#include "WorkerTransport.hpp"
#include <mutex>
#include <memory>
#include "assfire/util/amqp/RabbitMqConnector.hpp"

namespace assfire::tsp {
    class RabbitMQWorkerTransport : public WorkerTransport {
    public:
        RabbitMQWorkerTransport(const std::string& host, int port, const std::string& login, const std::string& password);

        void publishNewTask(const WorkerTask &task) override;

        void publishControlSignal(const WorkerControlSignal &signal) override;

    private:
        std::mutex task_publish_lock;
        util::RabbitMqConnector task_rabbit_mq_connector;
        std::unique_ptr<util::RabbitMqConnector::Publisher> task_publisher;

        std::mutex signal_publish_lock;
        util::RabbitMqConnector control_signal_rabbit_mq_connector;
        std::unique_ptr<util::RabbitMqConnector::Publisher> control_signal_publisher;
    };
}