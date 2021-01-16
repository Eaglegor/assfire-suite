#pragma once

#include <assfire/engine/router/model/RouteInfo.hpp>
#include <assfire/engine/router/model/Location.hpp>
#include <assfire/engine/router/model/RouteDetails.hpp>
#include "../../RouteProviderEngine.hpp"
#include "../../RoutingProfile.hpp"

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