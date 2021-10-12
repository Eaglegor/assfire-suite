#pragma once

#include "WorkerTaskPublisher.hpp"

namespace assfire::tsp::worker {
    class RabbitMQWorkerTaskPublisher : public WorkerTaskPublisher {
    public:
        void publishNewTask(const WorkerTask &task) override;

        void publishPauseEvent(const std::string &task_id) override;

        void publishStopEvent(const std::string &task_id) override;
    };
}