#include "RedisTspTaskStorage.hpp"

#include <spdlog/spdlog.h>
#include <cpp_redis/core/client.hpp>
#include "assfire/tsp/TspRedisConstants.hpp"

namespace assfire::tsp {
    RedisTspTaskStorage::RedisTspTaskStorage(std::shared_ptr<cpp_redis::client> redis_client)
            : redis_client(std::move(redis_client)) {}

    namespace {
        const int TSP_REDIS_TASK_EXPIRY_PERIOD_SECONDS = 3600 * 24;

        std::string formatTaskKey(const std::string &task_id) {
            return std::string(TSP_REDIS_WORKER_KEY_PREFIX) + task_id + std::string(TSP_REDIS_WORKER_TASK_KEY_SUFFIX);
        }

        std::string formatInProgressKey(const std::string &task_id) {
            return std::string(TSP_REDIS_WORKER_KEY_PREFIX) + task_id + std::string(TSP_REDIS_WORKER_TASK_PROGRESS_MARKER_SUFFIX);
        }
    }

    std::optional<TspTasksStorage::TspTask> RedisTspTaskStorage::fetchTask(const std::string &id) const {
        std::future<cpp_redis::reply> freply = redis_client->get(formatTaskKey(id));
        redis_client->sync_commit();

        cpp_redis::reply reply = freply.get();
        if (reply.is_error()) {
            SPDLOG_ERROR("Error while trying to retrieve task {} from cache: {}", id, reply.error());
            return std::nullopt;
        } else if (!reply.is_string()) {
            SPDLOG_INFO("Task with id {} not found in cache: {}", id);
            return std::nullopt;
        }

        TspTasksStorage::TspTask task;
        task.ParseFromString(reply.as_string());
        return task;
    }

    void RedisTspTaskStorage::saveTask(const std::string &id, TspTasksStorage::TspTask task) {
        redis_client->set(formatTaskKey(id), task.SerializeAsString(), [&](const cpp_redis::reply &rpl) {
            if (rpl.is_error()) {
                SPDLOG_ERROR("Error while saving task {} to Redis cache: {}", id, rpl.error());
            }
        });
        redis_client->expire(formatTaskKey(id), TSP_REDIS_TASK_EXPIRY_PERIOD_SECONDS);
        redis_client->commit();
    }

    void RedisTspTaskStorage::dropTask(const std::string &id) {
        redis_client->del({formatTaskKey(id)});
        redis_client->commit();
        markAsStopped(id);
    }

    bool RedisTspTaskStorage::isInProgress(const std::string &id) {
        std::future<cpp_redis::reply> freply = redis_client->get(formatInProgressKey(id));
        redis_client->sync_commit();

        cpp_redis::reply reply = freply.get();
        if (reply.is_error()) {
            SPDLOG_ERROR("Error while trying to retrieve \"in progress\" status for task {} from cache: {}", id, reply.error());
            return false;
        } else if (!reply.is_string()) {
            return false;
        }

        return reply.as_integer() != 0;
    }

    void RedisTspTaskStorage::markAsInProgress(const std::string &id) {
        redis_client->set(formatInProgressKey(id), "1", [&](const cpp_redis::reply &rpl) {
            if (rpl.is_error()) {
                SPDLOG_ERROR("Error while saving task {} to Redis cache: {}", id, rpl.error());
            }
        });
        redis_client->expire(formatInProgressKey(id), TSP_REDIS_TASK_EXPIRY_PERIOD_SECONDS);
        redis_client->commit();
    }

    void RedisTspTaskStorage::markAsStopped(const std::string &id) {
        redis_client->del({formatInProgressKey(id)});
        redis_client->commit();
    }
}