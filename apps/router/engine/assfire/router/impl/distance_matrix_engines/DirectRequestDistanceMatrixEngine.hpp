#pragma once

#include <memory>
#include "../../DistanceMatrixEngine.hpp"
#include "../../RouteProviderEngine.hpp"

namespace assfire::router
{
    class DirectRequestDistanceMatrixEngine : public DistanceMatrixEngine
    {
    public:
        DirectRequestDistanceMatrixEngine(std::unique_ptr <RouteProviderEngine> engine)
                :
                engine(std::move(engine))
        {
        }

        virtual RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        virtual RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const override;

        virtual IndexedLocation addLocation(const Location &location) override;

    private:
        std::vector <Location> known_locations;
        int matrix_tag;
        std::unique_ptr <RouteProviderEngine> engine;
    };
}