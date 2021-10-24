#include "NopSavedStateManager.hpp"

namespace assfire::tsp {

    std::optional<SavedStateManager::State> NopSavedStateManager::loadState(const std::string &task_id) {
        return std::nullopt;
    }

    void NopSavedStateManager::saveState(const std::string &task_id, const SavedStateManager::State &state) {

    }
}