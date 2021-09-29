#pragma once

#include "assfire/tsp/api/TspSolverType.hpp"
#include "assfire/tsp/api/TspSolverSettings.hpp"
#include "TspSolverEngine.hpp"


namespace assfire::tsp {
    class TspSolverFactory {
    public:
        TspSolverEngine createTspSolver(TspSolverType type, const TspSolverSettings& settings) const;
    };
}