#pragma once

#include <assfire/api/router/model/RouteInfo.hpp>
#include <assfire/api/router/model/Location.hpp>
#include <assfire/api/router/model/RouteDetails.hpp>
#include "assfire/api/router/RouteProviderEngine.hpp"
#include "assfire/api/router/model/RoutingProfile.hpp"
#include "assfire/api/router/RedisSerializer.hpp"
#include <random>
#include <functional>

namespace cpp_redis {
    class client;
}

namespace assfire::router {
    class RedisRouteProviderEngine : public RouteProviderEngine {
    public:

        RedisRouteProviderEngine(const RoutingProfile &routingProfile, std::unique_ptr<RedisSerializer> serializer, std::unique_ptr<RouteProviderEngine> backend_engine, const std::string &redis_host,
                                 std::size_t redis_port, bool force_update = false);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RoutingProfile routing_profile;
        std::unique_ptr<cpp_redis::client> redis_client;
        std::unique_ptr<RouteProviderEngine> backend_engine;
        std::unique_ptr<RedisSerializer> serializer;
        bool force_update;
    };
}