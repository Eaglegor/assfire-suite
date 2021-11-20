#include "TspCostTranslator.hpp"

namespace assfire::api::v1::tsp {
    TspCostTranslator::ApiTspCost TspCostTranslator::fromProto(const TspCostTranslator::ProtoTspCost &value) {
        return ApiTspCost(value.value());
    }

    TspCostTranslator::ProtoTspCost TspCostTranslator::toProto(const TspCostTranslator::ApiTspCost &value) {
        ProtoTspCost result;
        result.set_value(value.getValue());
        return result;
    }
}