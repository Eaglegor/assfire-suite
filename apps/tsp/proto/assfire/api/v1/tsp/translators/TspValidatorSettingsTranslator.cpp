#include "TspValidatorSettingsTranslator.hpp"

namespace assfire::api::v1::tsp {
    TspValidatorSettingsTranslator::ApiValidatorSettings TspValidatorSettingsTranslator::fromProto(const TspValidatorSettingsTranslator::ProtoValidatorSettings &value) {
        ApiValidatorSettings result;
        result.setType(fromProto(value.type()));
        return result;
    }

    TspValidatorSettingsTranslator::ProtoValidatorSettings TspValidatorSettingsTranslator::toProto(const TspValidatorSettingsTranslator::ApiValidatorSettings &value) {
        ProtoValidatorSettings result;
        result.set_type(toProto(value.getType()));
        return result;
    }

    TspValidatorSettingsTranslator::ApiValidatorType TspValidatorSettingsTranslator::fromProto(const TspValidatorSettingsTranslator::ProtoValidatorType &value) {
        switch (value) {
            case TSP_VALIDATOR_TYPE_UNSPECIFIED:
            case TSP_VALIDATOR_TYPE_DEFAULT:
                return ApiValidatorType::DEFAULT;
            case TSP_VALIDATOR_TYPE_FAIL_ON_ANY_VIOLATION:
                return ApiValidatorType::FAIL_ON_ANY_VIOLATION;
            default:
                throw std::invalid_argument("Unknown tsp validator type");
        }
    }

    TspValidatorSettingsTranslator::ProtoValidatorType TspValidatorSettingsTranslator::toProto(const TspValidatorSettingsTranslator::ApiValidatorType &value) {
        switch(value) {
            case ApiValidatorType::DEFAULT:
                return TSP_VALIDATOR_TYPE_DEFAULT;
            case ApiValidatorType::FAIL_ON_ANY_VIOLATION:
                return TSP_VALIDATOR_TYPE_FAIL_ON_ANY_VIOLATION;
            default:
                throw std::invalid_argument("Unknown tsp validator type");
        }
    }
}