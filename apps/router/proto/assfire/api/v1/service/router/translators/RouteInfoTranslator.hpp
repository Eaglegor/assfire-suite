#pragma once

#include <assfire/api/v1/service/router/router.grpc.pb.h>
#include <assfire/api/router/model/RoutingProfile.hpp>
#include "LocationTranslator.hpp"

namespace assfire::router::proto_translation {

    class RouteInfoTranslator {
    public:
        static RouteInfo fromProto(const assfire::api::v1::model::routing::RouteInfo &info) {
            return RouteInfo(info.distance(), info.duration());
        }

        static RouteDetails fromProtoWithDetails(const assfire::api::v1::model::routing::RouteInfo &info) {
            RouteInfo summary(info.distance(), info.duration());

            RouteDetails::Waypoints waypoints;
            waypoints.reserve(info.waypoints().size());
            for (const auto &wp : info.waypoints()) {
                waypoints.emplace_back(Coordinate::fromEncodedValue(wp.lat()), Coordinate::fromEncodedValue(wp.lon()));
            }

            return RouteDetails(summary, waypoints);
        }

        static assfire::api::v1::model::routing::RouteInfo
        toProto(const assfire::api::v1::model::routing::Location &origin, const assfire::api::v1::model::routing::Location &destination, const RouteInfo &info) {
            assfire::api::v1::model::routing::RouteInfo result;
            result.mutable_origin()->CopyFrom(origin);
            result.mutable_origin()->CopyFrom(destination);
            result.set_distance(info.getDistance());
            result.set_duration(info.getDuration());
            return result;
        }

        static assfire::api::v1::model::routing::RouteInfo toProto(const Location &origin, const Location &destination, const RouteInfo &info) {
            return toProto(LocationTranslator::toProto(origin), LocationTranslator::toProto(destination), info);
        }

        static assfire::api::v1::model::routing::RouteInfo
        toProto(const assfire::api::v1::model::routing::Location &origin, const assfire::api::v1::model::routing::Location &destination, const RouteDetails &details) {
            assfire::api::v1::model::routing::RouteInfo result = toProto(origin, destination, details.getSummary());
            for (const RouteDetails::Waypoint &waypoint : details.getWaypoints()) {
                auto *wp = result.add_waypoints();
                wp->set_lat(waypoint.getLatitude().encodedValue());
                wp->set_lon(waypoint.getLongitude().encodedValue());
            }
            return result;
        }

        static assfire::api::v1::model::routing::RouteInfo toProto(const Location &origin, const Location &destination, const RouteDetails &details) {
            return toProto(LocationTranslator::toProto(origin), LocationTranslator::toProto(destination), details);
        }
    };

}

