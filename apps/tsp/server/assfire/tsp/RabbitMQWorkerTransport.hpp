#pragma once

#include "WorkerTransport.hpp"
#include <mutex>
#include <memory>
#include <functional>
#include <future>
#include "assfire/util/amqp/RabbitMqConnector.hpp"

namespace assfire::tsp {
    class RabbitMQWorkerTransport : public WorkerTransport {
    public:

        RabbitMQWorkerTransport(const std::string& host, int port, const std::string& login, const std::string& password);

        virtual ~RabbitMQWorkerTransport();

        void publishNewTask(const WorkerTask &task) override;

        void publishControlSignal(const WorkerControlSignal &signal) override;

        void addWorkerStatusUpdateListener(const std::string &task_id, StatusUpdateListener listener) override;

        void removeStatusUpdateListener(const std::string &task_id) override;

    private:
        void subscribeForStatusUpdates();

        std::mutex task_publish_lock;
        util::RabbitMqConnector task_rabbit_mq_connector;
        std::unique_ptr<util::RabbitMqConnector::Publisher> task_publisher;

        std::mutex signal_publish_lock;
        util::RabbitMqConnector control_signal_rabbit_mq_connector;
        std::unique_ptr<util::RabbitMqConnector::Publisher> control_signal_publisher;

        util::RabbitMqConnector status_update_rabbit_mq_connector;
        std::future<void> status_update_control_state;

        std::mutex listeners_lock;
        std::unordered_map<std::string, StatusUpdateListener> status_listeners;
    };
}