#pragma once

#include <memory>
#include "assfire/tsp/worker/TaskProvider.hpp"
#include "assfire/api/v1/tsp/worker.pb.h"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {
    class RedisTaskProvider : public TaskProvider {
    public:
        RedisTaskProvider(std::unique_ptr<cpp_redis::client> client);

        std::optional<TspTask> retrieveTask(const std::string &task_id) override;

        bool isFinished(std::string &task_id) override;

        bool isPaused(std::string &task_id) override;

        bool tryLock(const std::string &task_id) override;

        void unlock(const std::string &task_id) override;

        void sendStarted(const std::string &task_id) override;

        void sendError(const std::string &task_id) override;

        void sendInProgress(const std::string &task_id) override;

        void sendPaused(const std::string &task_id) override;

        void sendInterrupted(const std::string &task_id) override;

        void sendFinished(const std::string &task_id) override;

        int incAttempts(const std::string &task_id) override;

    private:
        void sendStatusSignal(const std::string &task_id, api::v1::tsp::WorkerTspStatusUpdate_Type signal);

        std::unique_ptr<cpp_redis::client> client;
    };
}
