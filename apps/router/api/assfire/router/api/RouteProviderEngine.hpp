#pragma once

#include "Matrix.hpp"
#include "assfire/locations/api/Location.hpp"
#include "RouteInfo.hpp"
#include "RouteDetails.hpp"
#include "TripInfo.hpp"
#include "TripDetails.hpp"

namespace assfire::router {
    class RouteProviderEngine {
    public:
        using Location = locations::Location;
        using LocationsList = std::vector<Location>;
        using RouteDetailsVector = std::vector<RouteDetails>;
        using RouteInfoVector = std::vector<RouteInfo>;

        virtual ~RouteProviderEngine() = default;

        virtual RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const = 0;

        virtual RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const = 0;

        virtual Matrix<RouteInfo> getRouteInfoMatrix(const LocationsList &origins, const LocationsList &destinations) const {
            return Matrix<RouteInfo>(origins.size(), destinations.size(),
                                     [&](int i, int j) {
                                         return getSingleRouteInfo(origins.at(i), destinations.at(j));
                                     });
        };

        virtual Matrix<RouteDetails> getRouteDetailsMatrix(const LocationsList &origins, const LocationsList &destinations) const {
            return Matrix<RouteDetails>(origins.size(), destinations.size(),
                                        [&](int i, int j) {
                                            return getSingleRouteDetails(origins.at(i), destinations.at(j));
                                        });
        };

        virtual TripInfo getTripInfo(const LocationsList &locations) const {
            if(locations.size() < 2) return TripInfo::zero();
            RouteInfoVector result;
            result.reserve(locations.size() - 1);
            Distance total_distance = Distance::zero();
            TimeInterval total_duration = TimeInterval::zero();
            for (int i = 0; i < locations.size() - 1; ++i) {
                RouteInfo route = getSingleRouteInfo(locations[i], locations[i + 1]);
                total_distance += route.getDistance();
                total_duration += route.getDuration();
                result.push_back(route);
            }
            return TripInfo(total_distance, total_duration, result);
        };

        virtual TripDetails getTripDetails(const LocationsList &locations) const {
            if(locations.size() < 2) return TripDetails::zero();
            RouteDetailsVector result;
            result.reserve(locations.size() - 1);
            Distance total_distance = Distance::zero();
            TimeInterval total_duration = TimeInterval::zero();
            for (int i = 0; i < locations.size() - 1; ++i) {
                RouteDetails route = getSingleRouteDetails(locations[i], locations[i + 1]);
                total_distance += route.getDistance();
                total_duration += route.getDuration();
                result.push_back(route);
            }
            return TripDetails(total_distance, total_duration, result);
        };
    };
}