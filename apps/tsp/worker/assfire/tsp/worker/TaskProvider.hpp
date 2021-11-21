#pragma once

#include <optional>
#include "assfire/tsp/api/TspTask.hpp"

namespace assfire::tsp {
    class TaskProvider {
    public:
        virtual ~TaskProvider() = default;

        virtual std::optional<TspTask> retrieveTask(const std::string& task_id) = 0;

        virtual bool tryLock(const std::string& task_id) = 0;
        virtual void unlock(const std::string& task_id) = 0;

        virtual bool isFinished(std::string &task_id) = 0;
        virtual void sendStarted(const std::string& task_id) = 0;
        virtual void sendError(const std::string& task_id) = 0;
        virtual void sendInProgress(const std::string& task_id) = 0;
        virtual void sendStopped(const std::string& task_id) = 0;
        virtual void sendFinished(const std::string& task_id) = 0;
    };
}