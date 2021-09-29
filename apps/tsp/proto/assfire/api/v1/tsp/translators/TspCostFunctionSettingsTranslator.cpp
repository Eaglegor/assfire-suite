#include "TspCostFunctionSettingsTranslator.hpp"

namespace assfire::api::v1::tsp {
    TspCostFunctionSettingsTranslator::ApiCostFunctionSettings TspCostFunctionSettingsTranslator::fromProto(const TspCostFunctionSettingsTranslator::ProtoCostFunctionSettings &value) {
        ApiCostFunctionSettings result;
        result.setType(fromProto(value.type()));
        result.setTotalDistanceCostFunctionSettings(fromProto(value.total_distance_cost_function_settings()));
        return result;
    }

    TspCostFunctionSettingsTranslator::ProtoCostFunctionSettings TspCostFunctionSettingsTranslator::toProto(const TspCostFunctionSettingsTranslator::ApiCostFunctionSettings &value) {
        ProtoCostFunctionSettings result;
        result.set_type(toProto(value.getType()));
        result.mutable_total_distance_cost_function_settings()->CopyFrom(toProto(value.getTotalDistanceCostFunctionSettings()));
        return result;
    }

    TspCostFunctionSettingsTranslator::ApiCostFunctionType TspCostFunctionSettingsTranslator::fromProto(const TspCostFunctionSettingsTranslator::ProtoCostFunctionType &value) {
        switch (value) {
            case TSP_COST_FUNCTION_TYPE_UNSPECIFIED:
            case TSP_COST_FUNCTION_TYPE_DEFAULT:
                return ApiCostFunctionType::DEFAULT;
            case TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE:
                return ApiCostFunctionType::TOTAL_DISTANCE;
            default:
                throw std::invalid_argument("Unknown tsp cost function type: " + std::to_string(value));
        }
    }

    TspCostFunctionSettingsTranslator::ProtoCostFunctionType TspCostFunctionSettingsTranslator::toProto(const TspCostFunctionSettingsTranslator::ApiCostFunctionType &value) {
        switch (value) {
            case ApiCostFunctionType::DEFAULT:
                return TSP_COST_FUNCTION_TYPE_DEFAULT;
            case ApiCostFunctionType::TOTAL_DISTANCE:
                return TSP_COST_FUNCTION_TYPE_TOTAL_DISTANCE;
            default:
                throw std::invalid_argument("Unknown tsp cost function type: " + std::to_string(value));
        }
    }

    TspCostFunctionSettingsTranslator::ApiTotalDistanceCostFunctionSettings
    TspCostFunctionSettingsTranslator::fromProto(const TspCostFunctionSettingsTranslator::ProtoTotalDistanceCostFunctionSettings &value) {
        ApiTotalDistanceCostFunctionSettings result;
        result.setCostPerKm(value.cost_per_km());
        return result;
    }

    TspCostFunctionSettingsTranslator::ProtoTotalDistanceCostFunctionSettings
    TspCostFunctionSettingsTranslator::toProto(const TspCostFunctionSettingsTranslator::ApiTotalDistanceCostFunctionSettings &value) {
        ProtoTotalDistanceCostFunctionSettings result;
        result.set_cost_per_km(value.getCostPerKm());
        return result;
    }
}