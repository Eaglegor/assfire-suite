#pragma once

#include <functional>
#include "TspSolution.hpp"
#include "TspSolutionSession.hpp"

namespace assfire::tsp {
    class TspTask;

    class TspSolverApi {
    public:
        virtual TspSolutionSession solveTsp(const TspTask &task) const = 0;
    };
}

