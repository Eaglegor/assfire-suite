#pragma once

#include <assfire/api/router/model/RouteInfo.hpp>
#include <assfire/api/router/model/Location.hpp>
#include <assfire/api/router/model/RouteDetails.hpp>
#include "assfire/api/router/RouteProviderEngine.hpp"
#include "assfire/api/router/model/RoutingProfile.hpp"

namespace assfire::router {
    class CrowflightRouteProviderEngine : public RouteProviderEngine {
    public:
        CrowflightRouteProviderEngine(const RoutingProfile &routingProfile);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RoutingProfile routing_profile;
    };
}