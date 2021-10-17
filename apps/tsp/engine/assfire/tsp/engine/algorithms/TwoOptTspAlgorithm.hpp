#pragma once

#include "assfire/tsp/engine/TspAlgorithm.hpp"
#include "../TspEstimator.hpp"

namespace assfire::tsp {
    class TspEstimator;

    class TwoOptTspAlgorithm : public TspAlgorithm{
    public:
        TwoOptTspAlgorithm(TspEstimator estimator);

        void solveTsp(const TspTask &task, EngineTspSolutionController &solution_controller) const override;

    private:
        class State {
        public:
            State(int i, int j, int k, int size);

            int getI() const;

            int getJ() const;

            int getK() const;

            int getSize() const;

        private:
            int i, j, k, size;
        };

        TspSolution createInitialSolution(const TspTask &task, const TspAlgorithmStateContainer& saved_state,  const EngineTspSolutionController::SessionId& session_id) const;
        std::optional<State> createInitialState(const TspTask& task, const TspAlgorithmStateContainer& state_container, const EngineTspSolutionController::SessionId& session_id) const;
        std::optional<State> nextState(const std::optional<State>& state) const;

        void validateOrResetSavedState(const TspTask &task, TspAlgorithmStateContainer& container, const EngineTspSolutionController::SessionId& session_id) const;
        void saveState(const State& state, TspAlgorithmStateContainer& container, const TspSolution::Sequence &current_sequence, const TspTask& task) const;
        std::optional<State> loadState(const TspTask& task, const TspAlgorithmStateContainer& container) const;

        TspEstimator estimator;
    };
}