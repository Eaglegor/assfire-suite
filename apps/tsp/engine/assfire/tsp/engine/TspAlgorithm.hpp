#pragma once
#include "assfire/tsp/api/TspSolution.hpp"
#include "EngineTspSolutionController.hpp"

namespace assfire::tsp {
    class TspTask;

    class TspAlgorithm {
    public:
        virtual void solveTsp(const TspTask& task, EngineTspSolutionController &solution_controller) const = 0;
    };
}