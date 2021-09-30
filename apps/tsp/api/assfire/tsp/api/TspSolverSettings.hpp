#pragma once

#include "TspEstimatorSettings.hpp"
#include "TspRoutingSettings.hpp"

namespace assfire::tsp {
    class TspSolverSettings {
    public:
        TspSolverSettings(const TspRoutingSettings &routing_settings, const TspEstimatorSettings &estimator_settings) : routing_settings(routing_settings), estimator_settings(estimator_settings) {}

        const TspRoutingSettings &getRoutingSettings() const {
            return routing_settings;
        }

        const TspEstimatorSettings &getEstimatorSettings() const {
            return estimator_settings;
        }

    private:
        TspRoutingSettings routing_settings;
        TspEstimatorSettings estimator_settings;
    };
}