#pragma once

#include "model/RouteInfo.hpp"
#include "model/RouteDetails.hpp"
#include "model/IndexedLocation.hpp"

namespace assfire::router
{
    class DistanceMatrixEngine
    {
    public:
        virtual RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const = 0;

        virtual RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const = 0;

        virtual IndexedLocation addLocation(const Location &location) = 0;
    };
}