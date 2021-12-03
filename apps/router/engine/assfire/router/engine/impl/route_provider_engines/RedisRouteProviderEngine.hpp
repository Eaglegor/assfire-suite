#pragma once

#include <assfire/router/api/RouteInfo.hpp>
#include <assfire/router/api/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/api/RedisSerializer.hpp"
#include "assfire/router/engine/EngineMetricsCollector.hpp"
#include "CacheConnector.hpp"
#include <random>
#include <functional>

namespace assfire::router
{
    class CacheConnector;

    class RedisRouteProviderEngine : public RouteProviderEngine
    {
    public:

        RedisRouteProviderEngine(const RoutingProfile &routing_profile,
                                 std::unique_ptr<RedisSerializer> serializer,
                                 std::unique_ptr<RouteProviderEngine> backend_engine,
                                 std::unique_ptr<CacheConnector> redis_connector,
                                 bool force_update = false,
                                 EngineMetricsCollector metrics_collector = EngineMetricsCollector());

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RouteDetails retrieveSingleRouteDetails(const Location &origin, const Location &destination) const;

        RoutingProfile routing_profile;
        std::unique_ptr<CacheConnector> redis_client;
        std::unique_ptr<RouteProviderEngine> backend_engine;
        std::unique_ptr<RedisSerializer> serializer;
        bool force_update;
        mutable EngineMetricsCollector metrics_collector;
    };
}