#include "TspSolverFactory.hpp"
#include "algorithms/TwoOptTspAlgorithm.hpp"
#include <stdexcept>
#include <string>

namespace assfire::tsp {
    TspSolverEngine TspSolverFactory::createTspSolver(TspSolverType type, const TspSolverSettings &settings) const
    {
        switch(type) {
            case TspSolverType::TSP_SOLVER_TYPE_TWO_OPT:
                return TspSolverEngine(std::make_shared<TwoOptTspAlgorithm>());
            default:
                throw std::invalid_argument("Unsupported tsp solver type: " + std::to_string(static_cast<int>(type)));
        }
    }
};