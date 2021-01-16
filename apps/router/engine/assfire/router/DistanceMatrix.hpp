#pragma once

#include "RouteInfo.hpp"
#include "RouteDetails.hpp"
#include "DistanceMatrixEngine.hpp"
#include <memory>

namespace assfire::router
{
    class DistanceMatrix
    {
    public:
        using EnginePtr = std::shared_ptr<DistanceMatrixEngine>;

        DistanceMatrix(EnginePtr engine)
                : engine(engine)
        {}

        DistanceMatrix(const DistanceMatrix &rhs) = default;

        RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const
        {
            return engine->getRouteInfo(origin, destination);
        }

        RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const
        {
            return engine->getRouteDetails(origin, destination);
        }

        IndexedLocation addLocation(const Location &location)
        {
            return engine->addLocation(location);
        }

    private:
        std::shared_ptr <DistanceMatrixEngine> engine;
    };
}