#pragma once

#include "assfire/api/v1/tsp/worker.pb.h"

namespace assfire::tsp::worker {
    class WorkerTaskPublisher {
    public:
        using WorkerTask = assfire::api::v1::tsp::WorkerTask;

        virtual void publishNewTask(const WorkerTask& task) = 0;

        virtual void publishPauseEvent(const std::string& task_id) = 0;
        virtual void publishResumeEvent(const std::string& task_id) = 0;
        virtual void publishStopEvent(const std::string& task_id) = 0;
    };
}