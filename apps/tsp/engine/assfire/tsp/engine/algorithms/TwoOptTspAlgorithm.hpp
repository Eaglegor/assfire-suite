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

        TspSolution createInitialSolution(const TspTask &task, bool is_final) const;
        std::optional<State> createInitialState(int size, const TspAlgorithmStateContainer& state_container) const;
        std::optional<State> nextState(const std::optional<State>& state) const;

        void saveState(const State& state, TspAlgorithmStateContainer& container) const;
        std::optional<State> loadState(const TspAlgorithmStateContainer& container) const;

        TspEstimator estimator;
    };
}