#pragma once

#include <memory>
#include <assfire/router/api/DistanceMatrixErrorPolicy.hpp>
#include <cassert>
#include "assfire/router/api/DistanceMatrixEngine.hpp"
#include "assfire/router/api/RouteProviderEngine.hpp"

namespace assfire::router {
    class DirectRequestDistanceMatrixEngine : public DistanceMatrixEngine {
    public:
        DirectRequestDistanceMatrixEngine(std::unique_ptr<RouteProviderEngine> engine, Tag tag, DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY)
                : matrix_tag(tag),
                  engine(std::move(engine)),
                  error_policy(error_policy) {
            assert(tag != IndexedLocation::INVALID_TAG);
        }

        RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteInfo getRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getRouteDetails(const Location &origin, const Location &destination) const override;

        IndexedLocation addLocation(const Location &location, LocationType type) override;

    private:
        RouteDetails processError(const Location& from, const Location& to, const std::exception& e) const;

        Tag matrix_tag;
        std::unique_ptr<RouteProviderEngine> engine;
        DistanceMatrixErrorPolicy error_policy;
    };
}