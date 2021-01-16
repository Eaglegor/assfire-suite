#pragma once

#include "Matrix.hpp"

namespace assfire::router
{
    class RouteProviderEngine
    {
    public:
        virtual ~RouteProviderEngine() = default;

        virtual RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const = 0;

        virtual RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const = 0;

        virtual Matrix<RouteInfo> getRouteInfoMatrix(const std::vector <Location> &origins, const std::vector <Location> &destinations)
        {
            return Matrix(origins.size(), destinations.size(),
                          [&](int i, int j) {
                              return getSingleRouteInfo(origins.at(i), destinations.at(j));
                          });
        };

        virtual Matrix<RouteDetails> getRouteDetailsMatrix(const std::vector <Location> &origins, const std::vector <Location> &destinations)
        {
            return Matrix(origins.size(), destinations.size(),
                          [&](int i, int j) {
                              return getSingleRouteDetails(origins.at(i), destinations.at(j));
                          });
        };
    };
}