#pragma once

#include <memory>
#include "assfire/tsp/worker/SavedStateManager.hpp"
#include "assfire/util/redis/RedisConnector.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {
    class RedisSavedStateManager : public SavedStateManager {
    public:
        RedisSavedStateManager(util::RedisConnector& redis_connector);

        std::optional<State> loadState(const std::string &task_id) override;

        void saveState(const std::string &task_id, const State &state) override;

        void clearState(const std::string &task_id) override;

    private:
        util::RedisConnector& redis_connector;
    };
}