#pragma once

#include "assfire/tsp/api/TspSolverType.hpp"
#include "assfire/tsp/api/TspSolverSettings.hpp"
#include "assfire/tsp/api/TspSolver.hpp"


namespace assfire::tsp {
    class TspSolverFactory {
    public:
        TspSolver createTspSolver(TspSolverType type, const TspSolverSettings& settings) const;
    };
}