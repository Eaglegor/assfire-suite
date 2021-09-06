#include "TspSolverFactory.hpp"
#include "algorithms/TwoOptTspAlgorithm.hpp"
#include <stdexcept>
#include <string>

namespace assfire::tsp {
    TspSolver TspSolverFactory::createTspSolver(TspSolverType type, const TspSolverSettings &settings) const
    {
        switch(type) {
            case TspSolverType::TSP_SOLVER_TYPE_TWO_OPT:
                return TspSolver(std::make_shared<TwoOptTspAlgorithm>());
            default:
                throw std::invalid_argument("Unsupported tsp solver type: " + std::to_string(static_cast<int>(type)));
        }
    }
};