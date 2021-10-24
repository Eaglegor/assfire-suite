#pragma once

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include "WorkerTransport.hpp"
#include <mutex>

namespace assfire::tsp::worker {
    class RabbitMQWorkerTransport : public WorkerTransport {
    public:
        static const std::string TASK_QUEUE_NAME;
        static const std::string CONTROL_QUEUE_NAME;
        static const std::string RESULT_QUEUE_NAME;

        RabbitMQWorkerTransport(const std::string& host, int port, const std::string& login, const std::string& password);

        virtual ~RabbitMQWorkerTransport();

        void publishNewTask(const WorkerTask &task) override;

        void publishPauseEvent(const std::string &task_id) override;

        void publishStopEvent(const std::string &task_id) override;

    private:
        std::mutex publish_lock;
        amqp_socket_t* socket = nullptr;
        amqp_connection_state_t connection;
    };
}