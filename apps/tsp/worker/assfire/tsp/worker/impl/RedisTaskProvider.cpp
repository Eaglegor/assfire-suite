#include "RedisTaskProvider.hpp"
#include "TspWorkerConstants.hpp"
#include <cpp_redis/cpp_redis>
#include <spdlog/spdlog.h>
#include <chrono>
#include "assfire/api/v1/tsp/worker.pb.h"
#include "assfire/api/v1/tsp/translators/TspTaskTranslator.hpp"
#include "TspWorkerKeys.hpp"

using namespace assfire::api::v1::tsp;

namespace assfire::tsp {

    RedisTaskProvider::RedisTaskProvider(std::unique_ptr<cpp_redis::client> client)
            : client(std::move(client)) {}

    std::optional<TspTask> RedisTaskProvider::retrieveTask(const std::string &task_id) {
        auto ftask = client->get(taskKey(task_id));
        client->sync_commit();

        auto task = ftask.get();

        if (task.is_error()) {
            throw std::runtime_error(task.error());
        }
        if (task.is_null() || !task.is_string()) {
            return std::nullopt;
        }

        api::v1::tsp::TspTask protoTask;
        protoTask.ParseFromString(task.as_string());

        return api::v1::tsp::TspTaskTranslator::fromProto(protoTask);
    }

    bool RedisTaskProvider::tryLock(const std::string &task_id) {
        auto freply = client->getset(lockKey(task_id), TSP_WORKER_LOCK_VALUE);
        client->sync_commit();

        auto reply = freply.get();
        if (!reply.is_null()) {
            return false;
        }

        return true;
    }

    void RedisTaskProvider::unlock(const std::string &task_id) {
        auto freply = client->del({lockKey(task_id)});
        client->sync_commit();

        auto reply = freply.get();
        if (reply.is_error()) {
            SPDLOG_ERROR("Failed to clear lock for task {}", task_id);
        }
    }

    void RedisTaskProvider::sendStarted(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_STARTED);
    }

    void RedisTaskProvider::sendError(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_ERROR);
    }

    void RedisTaskProvider::sendInProgress(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_IN_PROGRESS);
    }

    void RedisTaskProvider::sendPaused(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_PAUSED);
    }

    void RedisTaskProvider::sendInterrupted(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_INTERRUPTED);
    }

    void RedisTaskProvider::sendFinished(const std::string &task_id) {
        sendStatusSignal(task_id, WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED);
    }

    bool RedisTaskProvider::isFinished(const std::string &task_id) {
        auto fstate = client->get(statusKey(task_id));
        client->sync_commit();

        auto state = fstate.get();

        return state.is_string() && state.as_string() == WorkerTspStatusUpdate_Type_Name(WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_FINISHED);
    }

    bool RedisTaskProvider::isPaused(const std::string &task_id) {
        auto fstate = client->get(statusKey(task_id));
        client->sync_commit();

        auto state = fstate.get();

        return state.is_string() && state.as_string() == WorkerTspStatusUpdate_Type_Name(WorkerTspStatusUpdate::WORKER_TSP_STATUS_UPDATE_TYPE_PAUSED);
    }

    void RedisTaskProvider::sendStatusSignal(const std::string &task_id, api::v1::tsp::WorkerTspStatusUpdate_Type signal) {
        auto freply = client->set(statusKey(task_id), WorkerTspStatusUpdate_Type_Name(signal));
        auto fka = client->setex(keepAliveKey(task_id), TSP_WORKER_KEEPALIVE_EXPIRY_SEC, TSP_WORKER_KEEPALIVE_VALUE);
        client->sync_commit();

        auto reply = freply.get();
        auto ka = fka.get();

        if (reply.is_error()) {
            throw std::runtime_error(reply.error());
        }
    }

    int RedisTaskProvider::incAttempts(const std::string &task_id) {
        auto freply = client->incr(attemptsKey(task_id));
        client->sync_commit();
        auto reply = freply.get();
        if (reply.is_error()) {
            throw std::runtime_error(reply.error());
        };
        return reply.as_integer();
    }
}
