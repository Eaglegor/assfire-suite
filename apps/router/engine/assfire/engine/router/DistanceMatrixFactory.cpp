#include "DistanceMatrixFactory.hpp"
#include "assfire/api/router/RouteProviderEngine.hpp"
#include "assfire/api/router/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"
#include "assfire/api/router/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.hpp"
#include "impl/route_provider_engines/CrowflightRouteProviderEngine.hpp"
#include "impl/route_provider_engines/EuclideanRouteProviderEngine.hpp"
#include "impl/route_provider_engines/OsrmRouteProviderEngine.hpp"
#include "impl/route_provider_engines/RandomRouteProviderEngine.hpp"
#include "impl/route_provider_engines/RedisRouteProviderEngine.hpp"
#include <stdexcept>

using namespace assfire::router;

namespace {
    std::unique_ptr<RouteProviderEngine> createEngine(RouterEngineType engine_type, const RoutingProfile &routing_profile, const RouteProviderSettings &settings, const RoutingContext &context) {
        switch (engine_type) {
            case assfire::router::RouterEngineType::CROWFLIGHT:
                return std::make_unique<CrowflightRouteProviderEngine>(routing_profile);
            case assfire::router::RouterEngineType::EUCLIDEAN:
                return std::make_unique<EuclideanRouteProviderEngine>(routing_profile);
            case assfire::router::RouterEngineType::OSRM:
                return std::make_unique<OsrmRouteProviderEngine>(context.getOsrmContext().getEndpoint(), routing_profile,
                                                                 settings.isRetrieveWaypoints() ? RouteProviderSettings::Osrm::Geometry::STRAIGHT_LINE : settings.getOsrmSettings().getGeometry());
            case assfire::router::RouterEngineType::RANDOM:
                return std::make_unique<RandomRouteProviderEngine>(routing_profile);
            default:
                throw std::runtime_error("Unknown route provider engine type");
        }
    }

    std::unique_ptr<RouteProviderEngine>
    createCacheWrappedEngine(RouterEngineType engine_type, const RoutingProfile &routing_profile, const RouteProviderSettings &settings, const RoutingContext &context) {
        if (context.getRedisContext().isCacheEnabled()) {
            if (!context.getRedisContext().getSerializerSupplier()) {
                throw std::invalid_argument("No serializer supplier is provided for redis cache");
            }
            return std::make_unique<RedisRouteProviderEngine>(routing_profile,
                                                              context.getRedisContext().getSerializerSupplier()(engine_type, routing_profile, settings),
                                                              createEngine(engine_type, routing_profile, settings, context),
                                                              context.getRedisContext().getHost(),
                                                              context.getRedisContext().getPort(),
                                                              settings.isForceUpdate());
        } else {
            return createEngine(engine_type, routing_profile, settings, context);
        }
    }
}

DistanceMatrix
DistanceMatrixFactory::createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings,
                                            const RoutingContext &context) {
    switch (caching_policy) {
        case DistanceMatrixCachingPolicy::NO_CACHING:
            return DistanceMatrix(std::make_shared<DirectRequestDistanceMatrixEngine>(createCacheWrappedEngine(engine_type, routing_profile, settings, context), ++tag_counter));
        case DistanceMatrixCachingPolicy::FULL_MATRIX_PRECACHING:
        case DistanceMatrixCachingPolicy::AUTO:
            return DistanceMatrix(std::make_shared<FullMatrixCacheDistanceMatrixEngine>(createCacheWrappedEngine(engine_type, routing_profile, settings, context), ++tag_counter));
        default:
            throw std::runtime_error("Unknown distance matrix caching policy");
    }
}

