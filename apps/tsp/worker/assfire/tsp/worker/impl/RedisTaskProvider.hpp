#pragma once

#include <memory>
#include "assfire/tsp/worker/TaskProvider.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {
    class RedisTaskProvider : public TaskProvider {
    public:
        RedisTaskProvider(std::unique_ptr<cpp_redis::client> client);

        std::optional<TspTask> retrieveTask(const std::string &task_id) override;

        bool tryLock(const std::string &task_id) override;

        void unlock(const std::string &task_id) override;

        void sendStarted(const std::string &task_id) override;

        void sendError(const std::string &task_id) override;

        void sendInProgress(const std::string &task_id) override;

        void sendStopped(const std::string &task_id) override;

        void sendFinished(const std::string &task_id) override;

    private:
        std::unique_ptr<cpp_redis::client> client;
    };
}
