#include "RedisSavedStateManager.hpp"
#include <cpp_redis/core/client.hpp>
#include <spdlog/spdlog.h>
#include "TspWorkerKeys.hpp"
#include "assfire/util/redis/RedisParsers.hpp"
#include "assfire/util/redis/RedisSerializers.hpp"

using namespace std::literals::chrono_literals;

namespace assfire::tsp
{
    RedisSavedStateManager::RedisSavedStateManager(util::RedisConnector &redis_connector)
            : redis_connector(redis_connector) {}


    std::optional<SavedStateManager::State> RedisSavedStateManager::loadState(const std::string &task_id) {
        try {
            return redis_connector.get<SavedStateManager::State>(
                    stateKey(task_id),
                    util::RedisProtoParser<SavedStateManager::State>(),
                    util::RedisRetryPolicy::retryFor(5, 5s)
            ).get();
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to retrieve saved state for task {}: {}", task_id, e.what());
            throw e;
        }
    }

    void RedisSavedStateManager::saveState(const std::string &task_id, const SavedStateManager::State &state) {
        try {
            redis_connector.set<SavedStateManager::State>(
                    stateKey(task_id),
                    state,
                    util::RedisProtoSerializer<SavedStateManager::State>(),
                    util::RedisRetryPolicy::retryFor(5, 5s),
                    util::RedisConnector::WriteMode::SYNC,
                    std::chrono::days(1)
            );
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to save state for task {}: {}", task_id, e.what());
            throw e;
        }
    }

    void RedisSavedStateManager::clearState(const std::string &task_id) {
        try {
            redis_connector.del(stateKey(task_id));
        } catch (const util::redis_exception &e) {
            SPDLOG_ERROR("Failed to clear saved state for task {}: {}", task_id, e.what());
            throw e;
        }
    }


}