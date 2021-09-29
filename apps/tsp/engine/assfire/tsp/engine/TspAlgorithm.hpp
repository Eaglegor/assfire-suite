#pragma once
#include "assfire/tsp/api/TspSolution.hpp"
#include "assfire/tsp/api/NopTspInterruptor.hpp"
#include "assfire/tsp/api/TspResultCallback.hpp"

namespace assfire::tsp {
    class TspTask;

    class TspAlgorithm {
    public:
        virtual TspSolution solveTsp(const TspTask& task, const TspInterruptor& interruptor = NopTspInterruptor()) const = 0;
        virtual void solveTsp(const TspTask& task, TspResultCallback resultCallback, const TspInterruptor& interruptor = NopTspInterruptor()) const = 0;
    };
}