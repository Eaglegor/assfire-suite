#pragma once

#include <assfire/api/router/RedisSerializer.hpp>

namespace assfire::router {
    class DefaultRedisSerializer : public RedisSerializer {
    public:
        std::string serializeKey(const Location &location, const Location &location1) override {
            return std::string();
        }

        std::string serializeRouteDetails(const Location &location, const Location &location1, const RouteDetails &details) override {
            return std::string();
        }

        RouteDetails deserializeRouteDetails(const Location &location, const Location &location1, const std::string &string) override {
            return RouteDetails();
        }

    private:

    };
}