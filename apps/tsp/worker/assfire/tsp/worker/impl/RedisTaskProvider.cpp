#include "RedisTaskProvider.hpp"
#include "TspImplConstants.hpp"
#include <cpp_redis/cpp_redis>
#include <spdlog/spdlog.h>
#include <chrono>
#include "assfire/api/v1/tsp/worker.pb.h"
#include "assfire/api/v1/tsp/translators/TspTaskTranslator.hpp"

namespace assfire::tsp
{
    namespace
    {
        std::string attemptsKey(const std::string &task_id) {
            return task_id + ".attempts";
        }

        std::string lockKey(const std::string &task_id) {
            return task_id + ".lock";
        }

        std::string stateKey(const std::string &task_id) {
            return task_id + ".state";
        }

        std::string keepAliveKey(const std::string &task_id) {
            return task_id + ".keepAlive";
        }

        std::string taskKey(const std::string &task_id) {
            return std::string(TSP_REDIS_WORKER_KEY_PREFIX) + task_id + std::string(TSP_REDIS_WORKER_SOLUTION_KEY_SUFFIX);
        }
    }

    RedisTaskProvider::RedisTaskProvider(std::unique_ptr<cpp_redis::client> client)
            : client(std::move(client)) {}

    std::optional<TspTask> RedisTaskProvider::retrieveTask(const std::string &task_id) {
        auto ftask = client->get(taskKey(task_id));
        auto fattempts = client->get(attemptsKey(task_id));
        auto fstate = client->get(stateKey(task_id));
        client->sync_commit();

        auto task = ftask.get();
        auto attempts = fattempts.get();
        auto state = fstate.get();

        if (state.is_error()) {
            throw std::runtime_error(state.error());
        }
        if (!state.is_null() && state.is_string()) {
            std::string st = state.as_string();
            if (st == "FINISHED") {
                return std::nullopt;
            }
        }

        if (attempts.is_error()) {
            throw std::runtime_error(attempts.error());
        }
        if (!attempts.is_null() && attempts.is_integer()) {
            if (state.as_integer() > TSP_MAX_ATTEMPTS) {
                return std::nullopt;
            }
        }

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
        auto freply = client->getset(lockKey(task_id), "1");
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
        auto freply = client->incr(attemptsKey(task_id));
        auto freply1 = client->set(stateKey(task_id), "STARTED");
        auto fka = client->setex(keepAliveKey(task_id), 30, "1");
        client->sync_commit();

        auto reply = freply.get();
        auto reply1 = freply.get();
        auto ka = fka.get();

        if (reply.is_error()) {
            throw std::runtime_error(reply.error());
        }
        if (reply1.is_error()) {
            throw std::runtime_error(reply1.error());
        }
    }

    void RedisTaskProvider::sendError(const std::string &task_id) {
        auto freply = client->set(stateKey(task_id), "ERROR");
        auto fka = client->setex(keepAliveKey(task_id), 30, "1");
        client->sync_commit();

        auto reply = freply.get();
        auto ka = fka.get();

        if (reply.is_error()) {
            throw std::runtime_error(reply.error());
        }
    }

    void RedisTaskProvider::sendInProgress(const std::string &task_id) {
        auto freply = client->set(stateKey(task_id), "IN_PROGRESS");
        auto fka = client->setex(keepAliveKey(task_id), 30, "1");
        client->sync_commit();

        auto reply = freply.get();
        auto ka = fka.get();

        if (reply.is_error()) {
            throw std::runtime_error(reply.error());
        }
    }

    void RedisTaskProvider::sendStopped(const std::string &task_id) {
        auto freply = client->set(stateKey(task_id), "STOPPED");
        auto fka = client->setex(keepAliveKey(task_id), 30, "1");
        client->sync_commit();

        auto reply = freply.get();
        auto ka = fka.get();

        if (reply.is_error()) {
            throw std::runtime_error(reply.error());
        }
    }

    void RedisTaskProvider::sendFinished(const std::string &task_id) {
        auto freply = client->set(stateKey(task_id), "FINISHED");
        auto fka = client->setex(keepAliveKey(task_id), 30, "1");
        client->sync_commit();

        auto reply = freply.get();
        auto ka = fka.get();

        if (reply.is_error()) {
            throw std::runtime_error(reply.error());
        }
    }
}
