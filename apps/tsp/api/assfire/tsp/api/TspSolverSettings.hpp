#pragma once

#include "TspEstimatorSettings.hpp"
#include "TspRoutingSettings.hpp"
#include "TspAlgorithmSettings.hpp"

namespace assfire::tsp {
    class TspSolverSettings {
    public:
        TspSolverSettings() = default;

        TspSolverSettings(const TspRoutingSettings &routing_settings, const TspEstimatorSettings &estimator_settings, const TspAlgorithmSettings &algorithm_settings)
                : routing_settings(routing_settings), estimator_settings(estimator_settings), algorithm_settings(algorithm_settings) {}

        const TspRoutingSettings &getRoutingSettings() const {
            return routing_settings;
        }

        const TspEstimatorSettings &getEstimatorSettings() const {
            return estimator_settings;
        }

        const TspAlgorithmSettings &getAlgorithmSettings() const {
            return algorithm_settings;
        }

    private:
        TspRoutingSettings routing_settings;
        TspEstimatorSettings estimator_settings;
        TspAlgorithmSettings algorithm_settings;
    };
}