#pragma once

#include <memory>
#include "assfire/engine/router/DistanceMatrixEngine.hpp"
#include "assfire/engine/router/RouteProviderEngine.hpp"
#include "../../Matrix.hpp"

namespace assfire::router {
    class FullMatrixCacheDistanceMatrixEngine : public DistanceMatrixEngine {
    public:
        FullMatrixCacheDistanceMatrixEngine(std::unique_ptr<RouteProviderEngine> engine, int tag)
                :
                matrix_tag(tag),
                engine(std::move(engine)) {
        }

        RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteInfo getRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getRouteDetails(const Location &origin, const Location &destination) const override;

        IndexedLocation addLocation(const Location &location) override;

    private:
        void initialize() const;

        bool initialized = false;
        std::vector<Location> known_locations;
        int matrix_tag;
        mutable std::unique_ptr<Matrix<RouteDetails>> route_details_cache;
        std::unique_ptr<RouteProviderEngine> engine;
    };
}