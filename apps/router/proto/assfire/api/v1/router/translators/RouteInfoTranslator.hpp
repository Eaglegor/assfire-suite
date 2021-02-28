#pragma once

#include <assfire/api/v1/router/concepts.pb.h>
#include <assfire/router/api/model/RoutingProfile.hpp>
#include <assfire/router/api/model/RouteInfo.hpp>
#include <assfire/router/api/model/RouteDetails.hpp>
#include "assfire/api/v1/concepts/translators/LocationTranslator.hpp"

namespace assfire::api::v1::router
{
    class RouteInfoTranslator
    {
    public:
        using ProtoRouteInfo = assfire::api::v1::router::RouteInfo;
        using ProtoLocation = assfire::api::v1::concepts::Location;

        using ApiRouteInfo = assfire::router::RouteInfo;
        using ApiRouteDetails = assfire::router::RouteDetails;

        static ApiRouteInfo fromProto(const ProtoRouteInfo &info);

        static ApiRouteDetails fromProtoWithDetails(const ProtoRouteInfo &info);

        static ProtoRouteInfo toProto(const ProtoLocation &origin, const ProtoLocation &destination, const ApiRouteInfo &info);

        static ProtoRouteInfo toProto(const Location &origin, const Location &destination, const ApiRouteInfo &info);

        static ProtoRouteInfo toProto(const ProtoLocation &origin, const ProtoLocation &destination, const ApiRouteDetails &details);

        static ProtoRouteInfo toProto(const Location &origin, const Location &destination, const ApiRouteDetails &details);
    };
}

