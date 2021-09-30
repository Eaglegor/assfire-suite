#pragma once

#include "assfire/tsp/engine/TspAlgorithm.hpp"
#include "../TspEstimator.hpp"

namespace assfire::tsp {
    class TspEstimator;

    class TwoOptTspAlgorithm : public TspAlgorithm{
    public:
        TwoOptTspAlgorithm(TspEstimator estimator);

        TspSolution solveTsp(const TspTask &task, const TspInterruptor &interruptor) const override;

        void solveTsp(const TspTask &task, TspResultCallback resultCallback, const TspInterruptor &interruptor) const override;

    private:
        TspSolution createInitialSolution(const TspTask &task, bool is_final) const;

        TspEstimator estimator;
    };
}