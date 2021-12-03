#pragma once

#include <assfire/router/api/RouteInfo.hpp>
#include <assfire/router/api/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include <random>
#include "assfire/router/engine/EngineMetricsCollector.hpp"

namespace assfire::router {
    class RandomRouteProviderEngine : public RouteProviderEngine {
    public:
        RandomRouteProviderEngine(const RoutingProfile &routingProfile, EngineMetricsCollector metrics_collector = EngineMetricsCollector());

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RouteInfo calculateRouteInfo(const Location &origin, const Location &destination) const;

        RoutingProfile routing_profile;
        std::random_device rd;
        mutable std::mt19937 gen;
        mutable std::uniform_real_distribution<> distr;
        mutable EngineMetricsCollector metrics_collector;
    };
}