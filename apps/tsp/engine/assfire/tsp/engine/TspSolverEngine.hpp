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

        TspSolutionSession solveTsp(const TspTask &task) const override;

    private:
        AlgorithmPtr tsp_algorithm;
    };
}

