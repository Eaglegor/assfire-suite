#pragma once

#include <assfire/router/api/model/RouteInfo.hpp>
#include <assfire/router/api/model/Location.hpp>
#include <assfire/router/api/model/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/model/RoutingProfile.hpp"

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