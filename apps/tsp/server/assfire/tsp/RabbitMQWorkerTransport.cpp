#include "RabbitMQWorkerTransport.hpp"
#include "assfire/tsp/TspAmqpConstants.hpp"
#include <spdlog/spdlog.h>

namespace assfire::tsp {
    using WorkerControlSignal = assfire::api::v1::tsp::WorkerControlSignal;

    RabbitMQWorkerTransport::RabbitMQWorkerTransport(const std::string &host, int port, const std::string &login, const std::string &password) :
            task_rabbit_mq_connector("WorkerTransport::Task"),
            control_signal_rabbit_mq_connector("WorkerTransport::ControlSignal"),
            status_update_rabbit_mq_connector("WorkerTransport::StatusUpdate") {
        SPDLOG_INFO("Initializing RabbitMQ worker transport");

        task_rabbit_mq_connector.connect(host, port, login, password);
        task_publisher = std::make_unique<util::RabbitMqConnector::Publisher>(task_rabbit_mq_connector.publish(
                TSP_AMQP_WORKER_TASK_QUEUE_NAME,
                TSP_AMQP_WORKER_TASK_EXCHANGE_NAME,
                TSP_AMQP_WORKER_TASK_CHANNEL_ID
        ));

        control_signal_rabbit_mq_connector.connect(host, port, login, password);
        control_signal_publisher = std::make_unique<util::RabbitMqConnector::Publisher>(control_signal_rabbit_mq_connector.publish(
                TSP_AMQP_WORKER_CONTROL_SIGNAL_QUEUE_NAME,
                TSP_AMQP_WORKER_CONTROL_SIGNAL_EXCHANGE_NAME,
                TSP_AMQP_WORKER_CONTROL_SIGNAL_CHANNEL_ID
        ));

        status_update_rabbit_mq_connector.connect(host, port, login, password);
        subscribeForStatusUpdates();

        SPDLOG_INFO("RabbitMQ worker transport successfully initialized");
    }

    void RabbitMQWorkerTransport::publishNewTask(const WorkerTask &task) {
        SPDLOG_INFO("Publishing worker task {} to RabbitMQ", task.task_id());

        SPDLOG_TRACE("Obtaining task publish lock");
        std::lock_guard<std::mutex> guard(task_publish_lock);
        SPDLOG_TRACE("Task publish lock obtained");

        std::size_t message_size = task.ByteSizeLong();
        void *buffer = malloc(message_size);
        task.SerializeToArray(buffer, message_size);
        task_publisher->publish(buffer, message_size);
        free(buffer);
    }

    void RabbitMQWorkerTransport::publishControlSignal(const WorkerControlSignal &signal) {
        SPDLOG_INFO("Publishing control signal for task {} to RabbitMQ", signal.task_id());

        SPDLOG_TRACE("Obtaining signal publish lock");
        std::lock_guard<std::mutex> guard(signal_publish_lock);
        SPDLOG_TRACE("Signal publish lock obtained");

        std::size_t message_size = signal.ByteSizeLong();
        void *buffer = malloc(message_size);
        signal.SerializeToArray(buffer, message_size);
        control_signal_publisher->publish(buffer, message_size);
        free(buffer);
    }

    void RabbitMQWorkerTransport::subscribeForStatusUpdates() {
        status_update_control_state = std::async(std::launch::async, [&] {
            status_update_rabbit_mq_connector.listen(
                    TSP_AMQP_WORKER_STATUS_UPDATE_QUEUE_NAME,
                    TSP_AMQP_WORKER_STATUS_UPDATE_EXCHANGE_NAME,
                    TSP_AMQP_WORKER_STATUS_UPDATE_CHANNEL_ID,
                    [&](const amqp_envelope_t_ &envelope) {
                        WorkerStatusUpdate status_update;
                        status_update.ParseFromArray(envelope.message.body.bytes, envelope.message.body.len);
                        {
                            std::lock_guard<std::mutex> guard(listeners_lock);
                            auto iter = status_listeners.find(status_update.task_id());
                            if (iter != status_listeners.end()) {
                                iter->second(status_update);
                            }
                        }
                    }
            );
        });
    }

    void RabbitMQWorkerTransport::addWorkerStatusUpdateListener(const std::string &task_id, StatusUpdateListener listener) {
        std::lock_guard<std::mutex> guard(listeners_lock);
        status_listeners.emplace(task_id, listener);
    }

    void RabbitMQWorkerTransport::removeStatusUpdateListener(const std::string &task_id) {
        std::lock_guard<std::mutex> guard(listeners_lock);
        status_listeners.erase(task_id);
    }

    RabbitMQWorkerTransport::~RabbitMQWorkerTransport() {
        status_update_rabbit_mq_connector.interrupt();
        status_update_control_state.wait();
    }
}
