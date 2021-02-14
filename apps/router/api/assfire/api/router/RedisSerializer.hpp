#pragma once

#include <string>
#include <assfire/api/router/model/Location.hpp>
#include <assfire/api/router/model/RouteDetails.hpp>

namespace assfire::router {
    class RedisSerializer {
    public:
        virtual ~RedisSerializer() = default;

        virtual std::string serializeKey(const Location &, const Location &) = 0;

        virtual std::string serializeRouteDetails(const Location &, const Location &, const RouteDetails &) = 0;

        virtual RouteDetails deserializeRouteDetails(const Location &, const Location &, const std::string &) = 0;
    };
}