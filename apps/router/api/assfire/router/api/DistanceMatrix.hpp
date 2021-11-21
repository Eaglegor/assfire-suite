#pragma once

#include "RouteInfo.hpp"
#include "RouteDetails.hpp"
#include "DistanceMatrixEngine.hpp"
#include <memory>

namespace assfire::router {
    class DistanceMatrix {
    public:
        using EnginePtr = std::shared_ptr<DistanceMatrixEngine>;

        DistanceMatrix(EnginePtr engine)
                : engine(std::move(engine)) {}

        DistanceMatrix(const DistanceMatrix &rhs) = default;
        DistanceMatrix(DistanceMatrix &&rhs) = default;

        RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const {
            return engine->getRouteInfo(origin, destination);
        }

        RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const {
            return engine->getRouteDetails(origin, destination);
        }

        RouteInfo getRouteInfo(const Location &origin, const Location &destination) const {
            return engine->getRouteInfo(origin, destination);
        }

        RouteDetails getRouteDetails(const Location &origin, const Location &destination) const {
            return engine->getRouteDetails(origin, destination);
        }

        IndexedLocation addLocation(const Location &location, DistanceMatrixEngine::LocationType type = DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION) {
            return engine->addLocation(location, type);
        }

    private:
        EnginePtr engine;
    };
}