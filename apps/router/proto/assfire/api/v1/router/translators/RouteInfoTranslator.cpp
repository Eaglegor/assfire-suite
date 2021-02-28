#include "RouteInfoTranslator.hpp"

using namespace assfire::api::v1::router;

using LocationTranslator = assfire::api::v1::concepts::LocationTranslator;

RouteInfoTranslator::ApiRouteInfo RouteInfoTranslator::fromProto(const ProtoRouteInfo &info)
{
    return ApiRouteInfo(Distance::fromMeters(info.distance().meters()), TimeInterval::fromSeconds(info.duration().seconds()));
}

RouteInfoTranslator::ApiRouteDetails RouteInfoTranslator::fromProtoWithDetails(const ProtoRouteInfo &info)
{
    ApiRouteInfo summary(Distance::fromMeters(info.distance().meters()), TimeInterval::fromSeconds(info.duration().seconds()));

    ApiRouteDetails::Waypoints waypoints;
    waypoints.reserve(info.waypoints().size());
    for (const auto &wp : info.waypoints()) {
        waypoints.emplace_back(Coordinate::fromEncodedValue(wp.encoded_latitude()), Coordinate::fromEncodedValue(wp.encoded_longitude()));
    }

    return ApiRouteDetails(summary, waypoints);
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const ProtoLocation &origin, const ProtoLocation &destination, const ApiRouteInfo &info)
{
    ProtoRouteInfo result;
    result.mutable_origin()->CopyFrom(origin);
    result.mutable_destination()->CopyFrom(destination);
    result.mutable_distance()->set_meters(info.getDistance().toMeters());
    result.mutable_duration()->set_seconds(info.getDuration().toSeconds());
    return result;
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const Location &origin, const Location &destination, const ApiRouteDetails &details)
{
    return toProto(LocationTranslator::toProto(origin), LocationTranslator::toProto(destination), details);
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const ProtoLocation &origin, const ProtoLocation &destination, const ApiRouteDetails &details)
{
    ProtoRouteInfo result = toProto(origin, destination, details.getSummary());
    for (const ApiRouteDetails::Waypoint &waypoint : details.getWaypoints()) {
        auto *wp = result.add_waypoints();
        wp->set_encoded_latitude(waypoint.getLatitude().encodedValue());
        wp->set_encoded_longitude(waypoint.getLongitude().encodedValue());
    }
    return result;
}

RouteInfoTranslator::ProtoRouteInfo RouteInfoTranslator::toProto(const Location &origin, const Location &destination, const ApiRouteInfo &info)
{
    return toProto(LocationTranslator::toProto(origin), LocationTranslator::toProto(destination), info);
}