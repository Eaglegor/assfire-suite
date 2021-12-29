#pragma once

#include <memory>
#include "assfire/tsp/worker/TaskProvider.hpp"
#include "assfire/api/v1/tsp/worker.pb.h"
#include "assfire/util/redis/RedisConnector.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {
    class RedisTaskProvider : public TaskProvider {
    public:
        RedisTaskProvider(util::RedisConnector& redis_connector);

        std::optional<TspTask> retrieveTask(const std::string &task_id) override;

        bool isFinished(const std::string &task_id) override;

        bool isPaused(const std::string &task_id) override;

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
        void sendStatusSignal(const std::string &task_id, api::v1::tsp::WorkerTspStatusUpdate_Type signal, bool sync);

        util::RedisConnector& redis_connector;
    };
}
