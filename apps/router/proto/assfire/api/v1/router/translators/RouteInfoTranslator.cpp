#include "RouteInfoTranslator.hpp"
#include <assfire/api/v1/concepts/translators/DistanceTranslator.hpp>
#include <assfire/api/v1/concepts/translators/TimeIntervalTranslator.hpp>

using namespace assfire::api::v1::router;
using namespace assfire::api::v1::concepts;

using LocationTranslator = assfire::api::v1::locations::LocationTranslator;

RouteInfoTranslator::ApiRouteInfo RouteInfoTranslator::fromProto(const ProtoRouteInfo &info)
{
    return ApiRouteInfo(DistanceTranslator::fromProto(info.distance()), TimeIntervalTranslator::fromProto(info.duration()));
}

RouteInfoTranslator::ApiRouteDetails RouteInfoTranslator::fromProtoWithDetails(const ProtoRouteInfo &info)
{
    ApiRouteInfo summary(DistanceTranslator::fromProto(info.distance()), TimeIntervalTranslator::fromProto(info.duration()));

    ApiRouteDetails::Waypoints waypoints;
    waypoints.reserve(info.waypoints().size());
    for (const auto &wp : info.waypoints()) {
        waypoints.emplace_back(LocationTranslator::fromProto(wp));
    }

    return ApiRouteDetails(summary, waypoints);
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const ProtoLocation &origin, const ProtoLocation &destination, const ApiRouteInfo &info)
{
    ProtoRouteInfo result;
    result.mutable_origin()->CopyFrom(origin);
    result.mutable_destination()->CopyFrom(destination);
    result.mutable_distance()->CopyFrom(DistanceTranslator::toProto(info.getDistance()));
    result.mutable_duration()->CopyFrom(TimeIntervalTranslator::toProto(info.getDuration()));
    return result;
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const ApiLocation &origin, const ApiLocation &destination, const ApiRouteDetails &details)
{
    return toProto(LocationTranslator::toProto(origin), LocationTranslator::toProto(destination), details);
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const ProtoLocation &origin, const ProtoLocation &destination, const ApiRouteDetails &details)
{
    ProtoRouteInfo result = toProto(origin, destination, details.getSummary());
    for (const ApiRouteDetails::Waypoint &waypoint : details.getWaypoints()) {
        result.add_waypoints()->CopyFrom(LocationTranslator::toProto(waypoint));
    }
    return result;
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const ApiLocation &origin, const ApiLocation &destination, const ApiRouteInfo &info)
{
    return toProto(LocationTranslator::toProto(origin), LocationTranslator::toProto(destination), info);
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const RouteInfoTranslator::ApiRouteInfo &info) {
    ProtoRouteInfo result;
    result.mutable_distance()->CopyFrom(DistanceTranslator::toProto(info.getDistance()));
    result.mutable_duration()->CopyFrom(TimeIntervalTranslator::toProto(info.getDuration()));
    return result;
}
