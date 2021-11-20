#pragma once

#include <optional>
#include <assfire/api/v1/tsp/state.pb.h>

namespace assfire::tsp {
    class SavedStateManager {
    public:
        using State = assfire::api::v1::tsp::TspAlgorithmState;

        virtual ~SavedStateManager() = default;

        virtual std::optional<State> loadState(const std::string &task_id) = 0;

        virtual void saveState(const std::string &task_id, const State &state) = 0;
        virtual void clearState(const std::string &task_id) = 0;
    };
}