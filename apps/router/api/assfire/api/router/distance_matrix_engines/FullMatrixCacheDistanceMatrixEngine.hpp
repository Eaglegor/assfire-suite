#pragma once

#include <memory>
#include <unordered_map>
#include <atomic>
#include <string>
#include <mutex>
#include <assfire/api/router/DistanceMatrixEngine.hpp>
#include <assfire/api/router/RouteProviderEngine.hpp>
#include <assfire/api/router/Matrix.hpp>

namespace assfire::router {
    class FullMatrixCacheDistanceMatrixEngine : public DistanceMatrixEngine {
    public:
        FullMatrixCacheDistanceMatrixEngine(std::unique_ptr<RouteProviderEngine> engine, Tag tag)
                :
                matrix_tag(tag),
                engine(std::move(engine)) {
        }

        RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteInfo getRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getRouteDetails(const Location &origin, const Location &destination) const override;

        IndexedLocation addLocation(const Location &location, LocationType type) override;

    private:
        void initialize() const;
        std::string encodeLocation(const Location& location) const;
        RouteInfo getCachedRouteInfo(int origin_id, int destination_id) const;
        RouteDetails getCachedRouteDetails(int origin_id, int destination_id) const;

        mutable std::atomic_bool initialized = false;
        std::vector<Location> known_locations;
        std::unordered_map<std::string, int> known_locations_mapping;
        Tag matrix_tag;
        mutable std::unique_ptr<Matrix<RouteDetails>> route_details_cache;
        std::unique_ptr<RouteProviderEngine> engine;
        mutable std::mutex cache_update_lock;
    };
}