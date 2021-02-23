#pragma once

#include <memory>
#include "../DistanceMatrixEngine.hpp"
#include "assfire/api/router/RouteProviderEngine.hpp"

namespace assfire::router
{
    class DirectRequestDistanceMatrixEngine : public DistanceMatrixEngine
    {
    public:
        DirectRequestDistanceMatrixEngine(std::unique_ptr <RouteProviderEngine> engine, Tag tag)
                :
                matrix_tag(tag),
                engine(std::move(engine))
        {
        }

        RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        RouteInfo getRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getRouteDetails(const Location &origin, const Location &destination) const override;

        IndexedLocation addLocation(const Location &location, LocationType type) override;

    private:
        Tag matrix_tag;
        std::unique_ptr <RouteProviderEngine> engine;
    };
}