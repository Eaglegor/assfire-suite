#pragma once

#include "assfire/engine/router/model/RouteInfo.hpp"
#include "assfire/engine/router/model/RouteDetails.hpp"
#include "assfire/engine/router/model/IndexedLocation.hpp"

namespace assfire::router
{
    class DistanceMatrixEngine
    {
    public:
        virtual ~DistanceMatrixEngine() = default;

        virtual RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const = 0;
        virtual RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const = 0;

        virtual RouteInfo getRouteInfo(const Location &origin, const Location &destination) const = 0;
        virtual RouteDetails getRouteDetails(const Location &origin, const Location &destination) const = 0;

        virtual IndexedLocation addLocation(const Location &location) = 0;
    };
}