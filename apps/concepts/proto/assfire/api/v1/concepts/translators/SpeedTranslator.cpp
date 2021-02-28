#include "SpeedTranslator.hpp"

using namespace assfire::api::v1::concepts;

SpeedTranslator::ApiSpeed SpeedTranslator::fromProto(const SpeedTranslator::ProtoSpeed &obj) {
    return ApiSpeed::fromMetersPerSecond(obj.meters_per_second());
}

SpeedTranslator::ProtoSpeed SpeedTranslator::toProto(const SpeedTranslator::ApiSpeed &obj) {
    ProtoSpeed result;
    result.set_meters_per_second(obj.toMetersPerSecond());
    return result;
}
