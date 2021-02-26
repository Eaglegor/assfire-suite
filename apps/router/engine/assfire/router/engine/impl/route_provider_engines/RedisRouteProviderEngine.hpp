#pragma once

#include <assfire/router/api/model/RouteInfo.hpp>
#include "../../../../../../../../libs/route_optimization_concepts/assfire/concepts/Location.hpp"
#include <assfire/router/api/model/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/model/RoutingProfile.hpp"
#include "assfire/router/api/RedisSerializer.hpp"
#include "CacheConnector.hpp"
#include <random>
#include <functional>

namespace assfire::router {
    class CacheConnector;

    class RedisRouteProviderEngine : public RouteProviderEngine {
    public:

        RedisRouteProviderEngine(const RoutingProfile &routing_profile, std::unique_ptr<RedisSerializer> serializer, std::unique_ptr<RouteProviderEngine> backend_engine, std::unique_ptr<CacheConnector> redis_connector, bool force_update = false);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RoutingProfile routing_profile;
        std::unique_ptr<CacheConnector> redis_client;
        std::unique_ptr<RouteProviderEngine> backend_engine;
        std::unique_ptr<RedisSerializer> serializer;
        bool force_update;
    };
}