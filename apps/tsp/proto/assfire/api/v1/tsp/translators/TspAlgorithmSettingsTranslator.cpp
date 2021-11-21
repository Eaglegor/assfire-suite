#include "TspAlgorithmSettingsTranslator.hpp"
#include "assfire/api/v1/tsp/translators/TspAlgorithmTypeTranslator.hpp"

namespace assfire::api::v1::tsp {
    TspAlgorithmSettingsTranslator::ApiAlgorithmSettings TspAlgorithmSettingsTranslator::fromProto(const TspAlgorithmSettingsTranslator::ProtoAlgorithmSettings &value) {
        return ApiAlgorithmSettings(TspAlgorithmTypeTranslator::fromProto(value.algorithm_type()));
    }

    TspAlgorithmSettingsTranslator::ProtoAlgorithmSettings TspAlgorithmSettingsTranslator::toProto(const TspAlgorithmSettingsTranslator::ApiAlgorithmSettings &value) {
        ProtoAlgorithmSettings result;
        result.set_algorithm_type(TspAlgorithmTypeTranslator::toProto(value.getAlgorithmType()));
        return result;
    }

}