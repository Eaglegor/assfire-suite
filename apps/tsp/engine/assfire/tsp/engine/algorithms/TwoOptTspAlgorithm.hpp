#pragma once

#include "assfire/tsp/api/TspAlgorithm.hpp"

namespace assfire::tsp {
    class TwoOptTspAlgorithm : public TspAlgorithm{
    public:
        TspSolution solveTsp(const TspTask &task, const TspInterruptor &interruptor) const override;

        void solveTsp(const TspTask &task, TspResultCallback resultCallback, const TspInterruptor &interruptor) const override;
    };
}