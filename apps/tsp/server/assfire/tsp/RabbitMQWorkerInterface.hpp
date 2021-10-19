#pragma once

#include <amqpcpp.h>
#include "WorkerInterface.hpp"

namespace assfire::tsp::worker {
    class RabbitMQWorkerInterface : public WorkerInterface {
    public:
        static const std::string TASK_QUEUE_NAME;
        static const std::string CONTROL_QUEUE_NAME;
        static const std::string RESULT_QUEUE_NAME;

        RabbitMQWorkerInterface();

        void publishNewTask(const WorkerTask &task) override;

        void publishPauseEvent(const std::string &task_id) override;

        void publishResumeEvent(const std::string &task_id) override;

        void publishStopEvent(const std::string &task_id) override;

    private:
        AMQP::Connection connection;
        AMQP::Channel channel;
    };
}