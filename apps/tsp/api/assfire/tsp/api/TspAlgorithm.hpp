#pragma once
#include "TspSolution.hpp"
#include "NopTspInterruptor.hpp"

namespace assfire::tsp {
    class TspTask;

    class TspAlgorithm {
    public:
        using TspResultCallback = std::function<void(const TspSolution&)>;

        virtual TspSolution solveTsp(const TspTask& task, const TspInterruptor& interruptor = NopTspInterruptor()) const = 0;
        virtual void solveTsp(const TspTask& task, TspResultCallback resultCallback, const TspInterruptor& interruptor = NopTspInterruptor()) const = 0;
    };
}