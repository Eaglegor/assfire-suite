#pragma once

#include "TspCostFunctionSettings.hpp"
#include "TspValidatorSettings.hpp"

namespace assfire::tsp {
    class TspEstimatorSettings {
    public:
        const TspCostFunctionSettings &getCostFunctionSettings() const {
            return cost_function_settings;
        }

        void setCostFunctionSettings(const TspCostFunctionSettings &cost_function_settings) {
            TspEstimatorSettings::cost_function_settings = cost_function_settings;
        }

        const TspValidatorSettings &getValidatorSettings() const {
            return validator_settings;
        }

        void setValidatorSettings(const TspValidatorSettings &validator_settings) {
            TspEstimatorSettings::validator_settings = validator_settings;
        }

    private:
        TspCostFunctionSettings cost_function_settings;
        TspValidatorSettings validator_settings;
    };
}