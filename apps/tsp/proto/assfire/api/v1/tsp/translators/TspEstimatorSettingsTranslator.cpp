#include "TspEstimatorSettingsTranslator.hpp"
#include "TspCostFunctionSettingsTranslator.hpp"
#include "TspValidatorSettingsTranslator.hpp"

namespace assfire::api::v1::tsp {
    TspEstimatorSettingsTranslator::ApiEstimatorSettings TspEstimatorSettingsTranslator::fromProto(const TspEstimatorSettingsTranslator::ProtoEstimatorSettings &value) {
        ApiEstimatorSettings result;
        result.setCostFunctionSettings(TspCostFunctionSettingsTranslator::fromProto(value.cost_function_settings()));
        result.setValidatorSettings(TspValidatorSettingsTranslator::fromProto(value.validator_settings()));
        return result;
    }

    TspEstimatorSettingsTranslator::ProtoEstimatorSettings TspEstimatorSettingsTranslator::toProto(const TspEstimatorSettingsTranslator::ApiEstimatorSettings &value) {
        ProtoEstimatorSettings result;
        result.mutable_cost_function_settings()->CopyFrom(TspCostFunctionSettingsTranslator::toProto(value.getCostFunctionSettings()));
        result.mutable_validator_settings()->CopyFrom(TspValidatorSettingsTranslator::toProto(value.getValidatorSettings()));
        return result;
    }
}