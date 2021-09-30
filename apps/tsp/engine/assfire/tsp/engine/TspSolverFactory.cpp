#include "TspSolverFactory.hpp"
#include "algorithms/TwoOptTspAlgorithm.hpp"
#include <stdexcept>
#include <string>

namespace assfire::tsp {
    TspSolverEngine TspSolverFactory::createTspSolver(TspAlgorithmType type, const TspSolverSettings &settings) const
    {
        switch(type) {
            case TspAlgorithmType::AUTO:
            case TspAlgorithmType::TWO_OPT:
                return TspSolverEngine(std::make_shared<TwoOptTspAlgorithm>(TspEstimator(nullptr, nullptr)));
            default:
                throw std::invalid_argument("Unsupported tsp solver type: " + std::to_string(static_cast<int>(type)));
        }
    }
};