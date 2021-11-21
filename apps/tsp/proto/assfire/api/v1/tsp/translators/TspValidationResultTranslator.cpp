#include "TspValidationResultTranslator.hpp"
#include <iterator>

namespace assfire::api::v1::tsp {
    TspValidationResultTranslator::ApiValidationResult TspValidationResultTranslator::fromProto(const TspValidationResultTranslator::ProtoValidationResult &value) {
        return ApiValidationResult(value.is_valid());
    }

    TspValidationResultTranslator::ProtoValidationResult TspValidationResultTranslator::toProto(const TspValidationResultTranslator::ApiValidationResult &value) {
        ProtoValidationResult result;
        result.set_is_valid(value.isValid());
        return result;
    }
}
