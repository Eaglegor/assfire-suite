#pragma once

#include "TspCostFunctionType.hpp"

namespace assfire::tsp {
    class TotalDistanceTspCostFunctionSettings {
    public:
        double getCostPerKm() const {
            return cost_per_km;
        }

        void setCostPerKm(double value) {
            cost_per_km = value;
        }

    private:
        double cost_per_km = 1; // Any non-zero value is acceptable as default
    };

    class TspCostFunctionSettings {
    public:
        TspCostFunctionType getType() const {
            return type;
        }

        void setType(TspCostFunctionType type) {
            this->type = type;
        }

        const TotalDistanceTspCostFunctionSettings &getTotalDistanceCostFunctionSettings() const {
            return totalDistanceCostFunctionSettings;
        }

        void setTotalDistanceCostFunctionSettings(const TotalDistanceTspCostFunctionSettings &total_distance_cost_function_settings) {
            totalDistanceCostFunctionSettings = total_distance_cost_function_settings;
        }

    private:
        TspCostFunctionType type = TspCostFunctionType::DEFAULT;
        TotalDistanceTspCostFunctionSettings totalDistanceCostFunctionSettings;
    };
}