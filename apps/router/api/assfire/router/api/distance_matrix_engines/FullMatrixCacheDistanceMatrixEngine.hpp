#pragma once

#include <memory>
#include <unordered_map>
#include <atomic>
#include <string>
#include <mutex>
#include <assfire/router/api/DistanceMatrixEngine.hpp>
#include <assfire/router/api/RouteProviderEngine.hpp>
#include <assfire/router/api/DistanceMatrixErrorPolicy.hpp>
#include <assfire/router/api/Matrix.hpp>
#include <cassert>

namespace assfire::router
{
    class FullMatrixCacheDistanceMatrixEngine : public DistanceMatrixEngine
    {
    public:
        FullMatrixCacheDistanceMatrixEngine(std::unique_ptr<RouteProviderEngine> engine, Tag tag, DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY)
                : matrix_tag(tag),
                  engine(std::move(engine)),
                  error_policy(error_policy),
                  route_details_cache(0, 0) {
            assert(tag != IndexedLocation::INVALID_TAG);
        }

        RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteInfo getRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getRouteDetails(const Location &origin, const Location &destination) const override;

        IndexedLocation addLocation(const Location &location, LocationType type) override;

        TripInfo getTripInfo(const LocationsList &locations) const override;

        TripDetails getTripDetails(const LocationsList &locations) const override;

        TripInfo getTripInfo(const IndexedLocationsList &locations) const override;

        TripDetails getTripDetails(const IndexedLocationsList &locations) const override;

    private:
        using LocationKey = std::string;
        using KnownLocationsMapping = std::unordered_map<LocationKey, int>;

        void initialize() const;

        static LocationKey encodeLocation(const Location &location);

        RouteInfo getCachedRouteInfo(int origin_id, int destination_id) const;

        RouteDetails getCachedRouteDetails(int origin_id, int destination_id) const;

        TripInfo getCachedTripInfo(const std::vector<int> &ids) const;

        TripDetails getCachedTripDetails(const std::vector<int> &ids) const;

        RouteDetails processError(const Location &from, const Location &to, const std::exception &e) const;

        Matrix<RouteDetails> processBatchError(const LocationsList &from, const LocationsList &to, const std::exception &e) const;

        TripInfo processTripInfoError(const LocationsList &locations, const std::exception &e) const;

        TripDetails processTripDetailsError(const LocationsList &locations, const std::exception &e) const;

        mutable std::atomic_bool is_initialized = false;
        std::vector<Location> known_locations;
        KnownLocationsMapping known_locations_mapping;
        Tag matrix_tag;
        mutable Matrix<RouteDetails> route_details_cache;
        std::unique_ptr<RouteProviderEngine> engine;
        mutable std::mutex cache_update_lock;
        DistanceMatrixErrorPolicy error_policy;
    };
}