#include "ClientDistanceMatrixFactory.hpp"
#include "assfire/api/router/RouteProviderEngine.hpp"
#include "assfire/api/router/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"
#include "assfire/api/router/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.hpp"
#include "GrpcRouteProviderEngine.hpp"
#include <stdexcept>

using namespace assfire::router;

namespace {
    std::unique_ptr<RouteProviderEngine> createEngine(const GrpcClient& client, RouterEngineType engine_type, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) {
        return std::make_unique<GrpcRouteProviderEngine>(client, engine_type, settings, routing_profile);
    }
}

DistanceMatrix
ClientDistanceMatrixFactory::createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) const {
    switch (caching_policy) {
        case DistanceMatrixCachingPolicy::NO_CACHING:
            return DistanceMatrix(std::make_shared<DirectRequestDistanceMatrixEngine>(createEngine(*grpc_client, engine_type, routing_profile, settings), ++tag_counter));
        case DistanceMatrixCachingPolicy::FULL_MATRIX_PRECACHING:
        case DistanceMatrixCachingPolicy::AUTO:
            return DistanceMatrix(std::make_shared<FullMatrixCacheDistanceMatrixEngine>(createEngine(*grpc_client, engine_type, routing_profile, settings), ++tag_counter));
        default:
            throw std::runtime_error("Unknown distance matrix caching policy");
    }
}

