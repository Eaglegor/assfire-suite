#include "DistanceMatrixFactory.hpp"
#include "RouteProviderEngine.hpp"
#include "impl/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"
#include "impl/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.hpp"
#include "impl/route_provider_engines/CrowflightRouteProviderEngine.hpp"
#include "impl/route_provider_engines/EuclideanRouteProviderEngine.hpp"
#include "impl/route_provider_engines/OsrmRouteProviderEngine.hpp"
#include "impl/route_provider_engines/RandomRouteProviderEngine.hpp"
#include <stdexcept>

using namespace assfire::router;

namespace {
    std::unique_ptr<RouteProviderEngine> createEngine(RouterEngineType engine_type, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) {
        switch (engine_type) {
            case assfire::router::RouterEngineType::CROWFLIGHT:
                return std::make_unique<CrowflightRouteProviderEngine>(routing_profile);
            case assfire::router::RouterEngineType::EUCLIDEAN:
                return std::make_unique<EuclideanRouteProviderEngine>(routing_profile);
            case assfire::router::RouterEngineType::OSRM:
                return std::make_unique<OsrmRouteProviderEngine>(settings.getOsrmSettings().getEndpoint(), routing_profile, settings.getOsrmSettings().getGeometry());
            case assfire::router::RouterEngineType::RANDOM:
                return std::make_unique<RandomRouteProviderEngine>(routing_profile);
            default:
                throw std::runtime_error("Unknown route provider engine type");
        }
    }
}

DistanceMatrix
DistanceMatrixFactory::createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) {
    switch (caching_policy) {
        case DistanceMatrixCachingPolicy::NO_CACHING:
            return DistanceMatrix(std::make_shared<DirectRequestDistanceMatrixEngine>(createEngine(engine_type, routing_profile, settings), ++tag_counter));
        case DistanceMatrixCachingPolicy::FULL_MATRIX_PRECACHING:
            return DistanceMatrix(std::make_shared<FullMatrixCacheDistanceMatrixEngine>(createEngine(engine_type, routing_profile, settings), ++tag_counter));
        default:
            throw std::runtime_error("Unknown distance matrix caching policy");
    }
}

