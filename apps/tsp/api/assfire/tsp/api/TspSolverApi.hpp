#pragma once

#include <functional>
#include "TspSolution.hpp"
#include "TspSolutionSession.hpp"
#include "TspSolutionListener.hpp"

namespace assfire::tsp {
    class TspTask;

    class TspSolverApi {
    public:
        virtual TspSolutionSession solveTsp(const TspTask &task, TspSolutionListener solution_listener) const = 0;
    };
}

