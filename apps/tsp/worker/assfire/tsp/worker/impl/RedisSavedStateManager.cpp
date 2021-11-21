#include "RedisSavedStateManager.hpp"
#include <cpp_redis/core/client.hpp>
#include <spdlog/spdlog.h>
#include "TspWorkerKeys.hpp"

namespace assfire::tsp {
    RedisSavedStateManager::RedisSavedStateManager(std::unique_ptr<cpp_redis::client> redis_client)
            : client(std::move(redis_client)) {}

    RedisSavedStateManager::~RedisSavedStateManager() {

    }

    std::optional<SavedStateManager::State> RedisSavedStateManager::loadState(const std::string &task_id) {
        auto fstate = client->get(stateKey(task_id));
        client->sync_commit();

        auto state = fstate.get();

        if (state.is_error()) {
            SPDLOG_ERROR("Failed to retrieve saved state for task {}: {}", task_id, state.error());
            return std::nullopt;
        }

        if(state.is_null()) {
            return std::nullopt;
        }

        SavedStateManager::State protoState;
        protoState.ParseFromString(state.as_string());

        return protoState;
    }

    void RedisSavedStateManager::saveState(const std::string &task_id, const SavedStateManager::State &state) {
        auto freply = client->set(stateKey(task_id), state.SerializeAsString());
        client->sync_commit();

        auto reply = freply.get();

        if (reply.is_error()) {
            SPDLOG_ERROR("Failed to save state for task {}: {}", task_id, reply.error());
        }
    }

    void RedisSavedStateManager::clearState(const std::string &task_id) {
        auto freply = client->del({stateKey(task_id)});
        client->sync_commit();

        auto reply = freply.get();

        if (reply.is_error()) {
            SPDLOG_ERROR("Failed to clear saved state for task {}: {}", task_id, reply.error());
        }
    }


}