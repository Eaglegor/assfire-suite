#include "DistanceTranslator.hpp"

using namespace assfire::api::v1::concepts;

DistanceTranslator::ApiDistance DistanceTranslator::fromProto(const DistanceTranslator::ProtoDistance &rhs) {
    return ApiDistance::fromMeters(rhs.meters());
}

DistanceTranslator::ProtoDistance DistanceTranslator::toProto(const DistanceTranslator::ApiDistance &rhs) {
    ProtoDistance result;
    result.set_meters(rhs.toMeters());
    return result;
}
