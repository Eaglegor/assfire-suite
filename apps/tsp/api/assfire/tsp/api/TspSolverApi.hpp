#pragma once

#include <functional>
#include "TspSolution.hpp"
#include "TspSolutionSession.hpp"

namespace assfire::tsp {
    class TspTask;

    class TspSolverApi {
    public:
        using TspSolutionListener = std::function<void(const TspSolution &)>;

        virtual TspSolutionSession solveTsp(const TspTask &task, TspSolutionListener solution_listener) const = 0;
    };
}

