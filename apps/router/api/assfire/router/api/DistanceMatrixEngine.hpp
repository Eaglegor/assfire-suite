#pragma once

#include "RouteInfo.hpp"
#include "RouteDetails.hpp"
#include "IndexedLocation.hpp"
#include "TripInfo.hpp"
#include "TripDetails.hpp"

namespace assfire::router
{
    class DistanceMatrixEngine
    {
    public:
        using Location = IndexedLocation::Location;
        using LocationsList = std::vector<Location>;
        using IndexedLocationsList = std::vector<IndexedLocation>;
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

        virtual TripInfo getTripInfo(const LocationsList& locations) const = 0;
        virtual TripDetails getTripDetails(const LocationsList& locations) const = 0;

        virtual TripInfo getTripInfo(const IndexedLocationsList& locations) const = 0;
        virtual TripDetails getTripDetails(const IndexedLocationsList& locations) const = 0;

        virtual IndexedLocation addLocation(const Location &location, LocationType type) = 0;
    };
}