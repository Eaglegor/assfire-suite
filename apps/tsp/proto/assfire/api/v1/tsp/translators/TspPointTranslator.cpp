#include "TspPointTranslator.hpp"
#include "assfire/api/v1/concepts/translators/LocationTranslator.hpp"

namespace assfire::api::v1::tsp {
    using namespace concepts;

    TspPointTranslator::ApiTspPoint TspPointTranslator::fromProto(const TspPointTranslator::ProtoTspPoint &value) {
        ApiTspPoint result;
        result.setLocation(LocationTranslator::fromProto(value.location()));
        return result;
    }

    TspPointTranslator::ProtoTspPoint TspPointTranslator::toProto(const TspPointTranslator::ApiTspPoint &value) {
        ProtoTspPoint result;
        result.mutable_location()->CopyFrom(LocationTranslator::toProto(value.getLocation()));
        return result;
    }
}