#pragma once

#include <assfire/engine/router/model/RouteInfo.hpp>
#include <assfire/engine/router/model/Location.hpp>
#include <assfire/engine/router/model/RouteDetails.hpp>
#include "../../RouteProviderEngine.hpp"
#include "../../RoutingProfile.hpp"
#include "../../RouteProviderSettings.hpp"
#include <random>
#include <functional>

namespace cpp_redis {
    class client;
}

namespace assfire::router {
    class RedisRouteProviderEngine : public RouteProviderEngine {
    public:


        RedisRouteProviderEngine(const RoutingProfile &routingProfile, std::unique_ptr<RouteProviderSettings::Redis::Serializer> serializer, std::unique_ptr<RouteProviderEngine> backend_engine, const std::string &redis_host,
                                 std::size_t redis_port, bool force_update = false);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RoutingProfile routing_profile;
        std::unique_ptr<cpp_redis::client> redis_client;
        std::unique_ptr<RouteProviderEngine> backend_engine;
        std::unique_ptr<RouteProviderSettings::Redis::Serializer> serializer;
        bool force_update;
    };
}