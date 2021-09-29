#pragma once

#include "assfire/tsp/api/TspSolverApi.hpp"
#include <memory>
#include <vector>
#include <functional>
#include "TspAlgorithm.hpp"

namespace assfire::tsp
{
    class TspTask;

    class TspSolverEngine : public TspSolverApi
    {
    public:
        using AlgorithmPtr = std::shared_ptr<TspAlgorithm>;

        TspSolverEngine(const AlgorithmPtr &tsp_algorithm);

        TspSolution solveTsp(const TspTask& task, const TspInterruptor& interruptor) const override;
        void solveTsp(const TspTask& task, TspResultCallback resultCallback, const TspInterruptor& interruptor) const override;

    private:
        AlgorithmPtr tsp_algorithm;
    };
}

