#include "RoutingProfileTranslator.hpp"

using namespace assfire::api::v1::router;

RoutingProfileTranslator::ApiRoutingProfile RoutingProfileTranslator::fromProto(const RoutingProfileTranslator::ProtoRoutingProfile &profile)
{
    return ApiRoutingProfile(fromProtoSpeed(profile.speed()));
}

RoutingProfileTranslator::ProtoRoutingProfile RoutingProfileTranslator::toProto(const RoutingProfileTranslator::ApiRoutingProfile &profile)
{
    ProtoRoutingProfile result;
    result.mutable_speed()->CopyFrom(toProtoSpeed(profile.getSpeed()));
    return result;
}

RoutingProfileTranslator::ApiSpeed RoutingProfileTranslator::fromProtoSpeed(const ProtoSpeed &velocity)
{
    return ApiSpeed::fromMetersPerSecond(velocity.meters_per_second());
}

RoutingProfileTranslator::ProtoSpeed RoutingProfileTranslator::toProtoSpeed(const ApiSpeed &speed)
{
    ProtoSpeed result;
    result.set_meters_per_second(speed.toMetersPerSecond());
    return result;
}
