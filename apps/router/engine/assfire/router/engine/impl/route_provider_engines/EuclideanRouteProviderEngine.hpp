#pragma once

#include <assfire/router/api/RouteInfo.hpp>
#include "assfire/concepts/Location.hpp"
#include <assfire/router/api/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/RoutingProfile.hpp"

namespace assfire::router {
    class EuclideanRouteProviderEngine : public RouteProviderEngine {
    public:
        EuclideanRouteProviderEngine(const RoutingProfile &routingProfile);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RoutingProfile routing_profile;
    };
}