#pragma once

#include <memory>
#include "assfire/tsp/worker/SavedStateManager.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {
    class RedisSavedStateManager : public SavedStateManager {
    public:
        RedisSavedStateManager(std::unique_ptr<cpp_redis::client> redis_client);

        virtual ~RedisSavedStateManager();

        std::optional<State> loadState(const std::string &task_id) override;

        void saveState(const std::string &task_id, const State &state) override;

        void clearState(const std::string &task_id) override;

    private:
        std::unique_ptr<cpp_redis::client> client;
    };
}