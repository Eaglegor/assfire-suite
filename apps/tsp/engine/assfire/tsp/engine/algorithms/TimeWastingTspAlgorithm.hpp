#pragma once

#include "assfire/tsp/engine/TspEstimator.hpp"
#include "assfire/tsp/engine/TspAlgorithm.hpp"

namespace assfire::tsp {
    class TimeWastingTspAlgorithm : public TspAlgorithm {
    public:
        TimeWastingTspAlgorithm(TspEstimator estimator);

        void solveTsp(const TspTask &task, EngineTspSolutionController &solution_controller) const override;

    private:
        class State {
        public:
            State(int counter);

            int getCounter() const;

        private:
            int counter;
        };

        TspSolution createInitialSolution(const TspTask &task, const TspAlgorithmStateContainer &saved_state, const EngineTspSolutionController::SessionId &session_id) const;

        std::optional<State> createInitialState(const TspTask &task, const TspAlgorithmStateContainer &state_container, const EngineTspSolutionController::SessionId &session_id) const;

        std::optional<State> nextState(const std::optional<State> &state) const;

        void validateOrResetSavedState(const TspTask &task, TspAlgorithmStateContainer &container, const EngineTspSolutionController::SessionId &session_id) const;

        void saveState(const State &state, TspAlgorithmStateContainer &container, const TspSolution::Sequence &current_sequence, const TspTask &task) const;

        std::optional<State> loadState(const TspTask &task, const TspAlgorithmStateContainer &container) const;

        TspEstimator estimator;
    };
}