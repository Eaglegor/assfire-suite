#include "NopTspTasksStorage.hpp"
#include <spdlog/spdlog.h>

namespace assfire::tsp {

    std::optional<NopTspTasksStorage::TspTask> NopTspTasksStorage::fetchTask(const std::string &id) const {
        SPDLOG_INFO("Fetching task {} from tasks storage", id);
        return std::nullopt;
    }

    void NopTspTasksStorage::saveTask(const std::string &id, TspTasksStorage::TspTask task) {
        SPDLOG_INFO("Saving task {} to tasks storage", id);
    }

    void NopTspTasksStorage::dropTask(const std::string &id) {
        SPDLOG_INFO("Dropping task {} from tasks storage", id);
    }

    void NopTspTasksStorage::markAsInProgress(const std::string &id) {
        SPDLOG_INFO("Marking task {} as in progress", id);
    }

    void NopTspTasksStorage::markAsStopped(const std::string &id) {
        SPDLOG_INFO("Marking task {} as stopped", id);
    }

    bool NopTspTasksStorage::isInProgress(const std::string &id) {
        SPDLOG_INFO("Checking if task {} in progress", id);
        return false;
    }
}