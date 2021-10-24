#pragma once

#include <memory>
#include "TspTasksStorage.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {

    class RedisTspTaskStorage : public TspTasksStorage {
    public:
        RedisTspTaskStorage(std::shared_ptr<cpp_redis::client> redis_client);

        std::optional<TspTask> fetchTask(const std::string &id) const override;

        void saveTask(const std::string &id, TspTask task) override;

        void dropTask(const std::string &id) override;

        bool isInProgress(const std::string &id) override;

        void markAsInProgress(const std::string &id) override;

        void markAsStopped(const std::string &id) override;

    private:
        std::shared_ptr<cpp_redis::client> redis_client;
    };
}