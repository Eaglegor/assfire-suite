#pragma once

#include "Matrix.hpp"
#include "model/Location.hpp"
#include "model/RouteInfo.hpp"
#include "model/RouteDetails.hpp"

namespace assfire::router
{
    class RouteProviderEngine
    {
    public:
        using Locations = std::vector<Location>;

        virtual ~RouteProviderEngine() = default;

        virtual RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const = 0;

        virtual RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const = 0;

        virtual Matrix<RouteInfo> getRouteInfoMatrix(const Locations &origins, const Locations &destinations)
        {
            return Matrix<RouteInfo>(origins.size(), destinations.size(),
                          [&](int i, int j) {
                              return getSingleRouteInfo(origins.at(i), destinations.at(j));
                          });
        };

        virtual Matrix<RouteDetails> getRouteDetailsMatrix(const Locations &origins, const Locations &destinations)
        {
            return Matrix<RouteDetails>(origins.size(), destinations.size(),
                          [&](int i, int j) {
                              return getSingleRouteDetails(origins.at(i), destinations.at(j));
                          });
        };
    };
}