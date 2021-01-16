#include "DistanceMatrixFactory.hpp"
#include "RouteProviderEngine.hpp"
#include "impl/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"
#include "impl/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.hpp"
#include "impl/route_provider_engines/CrowflightRouteProviderEngine.hpp"
#include "impl/route_provider_engines/EuclideanRouteProviderEngine.hpp"
#include "impl/route_provider_engines/OsrmRouteProviderEngine.hpp"
#include "impl/route_provider_engines/RandomRouteProviderEngine.hpp"
#include "impl/route_provider_engines/RedisRouteProviderEngine.hpp"
#include <stdexcept>
#include <cassert>

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

    std::unique_ptr<RouteProviderEngine> createCacheWrappedEngine(RouterEngineType engine_type, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) {
        if (settings.isEnableRedisCache()) {
            assert(settings.getRedisSettings().getSerializerSupplier());
            return std::make_unique<RedisRouteProviderEngine>(routing_profile, settings.getRedisSettings().getSerializerSupplier()(), createEngine(engine_type, routing_profile, settings),
                                                              settings.getRedisSettings().getHost(), settings.getRedisSettings().getPort(), settings.getRedisSettings().isForceUpdate());
        } else {
            return createEngine(engine_type, routing_profile, settings);
        }
    }
}

DistanceMatrix
DistanceMatrixFactory::createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) {
    switch (caching_policy) {
        case DistanceMatrixCachingPolicy::NO_CACHING:
            return DistanceMatrix(std::make_shared<DirectRequestDistanceMatrixEngine>(createCacheWrappedEngine(engine_type, routing_profile, settings), ++tag_counter));
        case DistanceMatrixCachingPolicy::FULL_MATRIX_PRECACHING:
            return DistanceMatrix(std::make_shared<FullMatrixCacheDistanceMatrixEngine>(createCacheWrappedEngine(engine_type, routing_profile, settings), ++tag_counter));
        default:
            throw std::runtime_error("Unknown distance matrix caching policy");
    }
}

