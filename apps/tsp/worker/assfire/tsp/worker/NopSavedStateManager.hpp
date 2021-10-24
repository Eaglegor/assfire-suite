#pragma once

#include "SavedStateManager.hpp"

namespace assfire::tsp {
    class NopSavedStateManager : public SavedStateManager {
    public:
        std::optional<State> loadState(const std::string &task_id) override;

        void saveState(const std::string &task_id, const State &state) override;
    };
}