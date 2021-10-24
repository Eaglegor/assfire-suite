#include "NopTspTaskStorage.hpp"
#include <spdlog/spdlog.h>

namespace assfire::tsp {

    std::optional<NopTspTaskStorage::TspTask> NopTspTaskStorage::fetchTask(const std::string &id) const {
        SPDLOG_INFO("Fetching task {} from tasks storage", id);
        return std::nullopt;
    }

    void NopTspTaskStorage::saveTask(const std::string &id, TspTaskStorage::TspTask task) {
        SPDLOG_INFO("Saving task {} to tasks storage", id);
    }

    void NopTspTaskStorage::dropTask(const std::string &id) {
        SPDLOG_INFO("Dropping task {} from tasks storage", id);
    }

    void NopTspTaskStorage::markAsInProgress(const std::string &id) {
        SPDLOG_INFO("Marking task {} as in progress", id);
    }

    void NopTspTaskStorage::markAsStopped(const std::string &id) {
        SPDLOG_INFO("Marking task {} as stopped", id);
    }

    bool NopTspTaskStorage::isInProgress(const std::string &id) {
        SPDLOG_INFO("Checking if task {} in progress", id);
        return false;
    }
}