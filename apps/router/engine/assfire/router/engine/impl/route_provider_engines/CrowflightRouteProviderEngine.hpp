#pragma once

#include <assfire/router/api/RouteInfo.hpp>
#include <assfire/router/api/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/engine/EngineMetricsCollector.hpp"

namespace assfire::router {
    class CrowflightRouteProviderEngine : public RouteProviderEngine {
    public:
        CrowflightRouteProviderEngine(const RoutingProfile &routingProfile, EngineMetricsCollector metrics_collector = EngineMetricsCollector());

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RouteInfo calculateRouteInfo(const Location& origin, const Location& destination) const;

        RoutingProfile routing_profile;
        mutable EngineMetricsCollector metrics_collector;
    };
}