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

namespace assfire::router {
    class FullMatrixCacheDistanceMatrixEngine : public DistanceMatrixEngine {
    public:
        FullMatrixCacheDistanceMatrixEngine(std::unique_ptr<RouteProviderEngine> engine, Tag tag, DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY)
                :
                matrix_tag(tag),
                engine(std::move(engine)),
                error_policy(error_policy) {
        }

        RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteInfo getRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getRouteDetails(const Location &origin, const Location &destination) const override;

        IndexedLocation addLocation(const Location &location, LocationType type) override;

    private:
        void initialize() const;

        std::string encodeLocation(const Location &location) const;

        RouteInfo getCachedRouteInfo(int origin_id, int destination_id) const;

        RouteDetails getCachedRouteDetails(int origin_id, int destination_id) const;

        RouteDetails processError(const Location& from, const Location& to, const std::exception& e) const;

        mutable std::atomic_bool is_initialized = false;
        std::vector<Location> known_locations;
        std::unordered_map<std::string, int> known_locations_mapping;
        Tag matrix_tag;
        mutable std::unique_ptr<Matrix<RouteDetails>> route_details_cache;
        std::unique_ptr<RouteProviderEngine> engine;
        mutable std::mutex cache_update_lock;
        DistanceMatrixErrorPolicy error_policy;
    };
}