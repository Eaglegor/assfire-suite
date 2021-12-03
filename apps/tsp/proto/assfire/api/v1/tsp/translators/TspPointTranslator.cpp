#include "TspPointTranslator.hpp"
#include "assfire/api/v1/locations/translators/LocationTranslator.hpp"

namespace assfire::api::v1::tsp {
    using namespace concepts;

    TspPointTranslator::ApiTspPoint TspPointTranslator::fromProto(const TspPointTranslator::ProtoTspPoint &value) {
        return ApiTspPoint(value.id(), locations::LocationTranslator::fromProto(value.location()));
    }

    TspPointTranslator::ProtoTspPoint TspPointTranslator::toProto(const TspPointTranslator::ApiTspPoint &value) {
        ProtoTspPoint result;
        result.set_id(value.getId());
        result.mutable_location()->CopyFrom(locations::LocationTranslator::toProto(value.getLocation()));
        return result;
    }
}