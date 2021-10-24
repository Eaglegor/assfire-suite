#pragma once
#include "TspTasksStorage.hpp"

namespace assfire::tsp {
    class NopTspTasksStorage : public TspTasksStorage {
    public:
        std::optional<TspTask> fetchTask(const std::string &id) const override;

        void saveTask(const std::string &id, TspTask task) override;

        void dropTask(const std::string &id) override;

        void markAsInProgress(const std::string& id) override;
        void markAsStopped(const std::string& id) override;

        bool isInProgress(const std::string &id) override;
    };
}