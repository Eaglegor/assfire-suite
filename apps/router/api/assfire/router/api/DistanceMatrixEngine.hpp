#pragma once

#include "model/RouteInfo.hpp"
#include "model/RouteDetails.hpp"
#include "model/IndexedLocation.hpp"

namespace assfire::router
{
    class DistanceMatrixEngine
    {
    public:
        enum class LocationType {
            ORIGIN,
            DESTINATION,
            ORIGIN_AND_DESTINATION
        };

        using Tag = IndexedLocation::IndexTag;
        virtual ~DistanceMatrixEngine() = default;

        virtual RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const = 0;
        virtual RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const = 0;

        virtual RouteInfo getRouteInfo(const Location &origin, const Location &destination) const = 0;
        virtual RouteDetails getRouteDetails(const Location &origin, const Location &destination) const = 0;

        virtual IndexedLocation addLocation(const Location &location, LocationType type) = 0;
    };
}