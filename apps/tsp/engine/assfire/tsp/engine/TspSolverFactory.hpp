#pragma once

#include "assfire/tsp/api/TspAlgorithmType.hpp"
#include "assfire/tsp/api/TspSolverSettings.hpp"
#include "TspSolverEngine.hpp"


namespace assfire::tsp {
    class TspSolverFactory {
    public:
        TspSolverEngine createTspSolver(TspAlgorithmType type, const TspSolverSettings& settings) const;
    };
}