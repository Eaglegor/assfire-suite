#include "RoutingProfileTranslator.hpp"

#include <assfire/api/v1/concepts/translators/SpeedTranslator.hpp>

using namespace assfire::api::v1::router;
using namespace assfire::api::v1::concepts;

RoutingProfileTranslator::ApiRoutingProfile RoutingProfileTranslator::fromProto(const RoutingProfileTranslator::ProtoRoutingProfile &profile)
{
    return ApiRoutingProfile(SpeedTranslator::fromProto(profile.speed()));
}

RoutingProfileTranslator::ProtoRoutingProfile RoutingProfileTranslator::toProto(const RoutingProfileTranslator::ApiRoutingProfile &profile)
{
    ProtoRoutingProfile result;
    result.mutable_speed()->CopyFrom(SpeedTranslator::toProto(profile.getSpeed()));
    return result;
}
