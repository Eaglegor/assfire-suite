#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "TspAlgorithm.hpp"

namespace assfire::tsp
{
    class TspTask;

    class TspSolver
    {
    public:
        using AlgorithmPtr = std::shared_ptr<TspAlgorithm>;
        using TspResultCallback = TspAlgorithm::TspResultCallback;

        TspSolver(const AlgorithmPtr &tsp_algorithm);

        TspSolution solveTsp(const TspTask& task, const TspInterruptor& interruptor) const;
        void solveTsp(const TspTask& task, TspResultCallback resultCallback, const TspInterruptor& interruptor) const;

    private:
        AlgorithmPtr tsp_algorithm;
    };
}

