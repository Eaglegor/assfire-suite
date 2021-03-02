#include "RouterEngine.hpp"

using namespace assfire::router;

DistanceMatrix RouterEngine::createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings,
                                   const RoutingContext &context, DistanceMatrixErrorPolicy error_policy) const {
    return distance_matrix_factory.createDistanceMatrix(engine_type, caching_policy, routing_profile, settings, context, error_policy);
}

RouteInfo RouterEngine::getRouteInfo(const assfire::Location &origin, const assfire::Location &destination, RouterEngineType engine_type, const RoutingProfile &routing_profile,
                                     const RouteProviderSettings &settings, const RoutingContext &context) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, context, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getRouteInfo(origin, destination);
}

RouteDetails RouterEngine::getRouteDetails(const assfire::Location &origin, const assfire::Location &destination, RouterEngineType engine_type, const RoutingProfile &routing_profile,
                                           const RouteProviderSettings &settings, const RoutingContext &context) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, context, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getRouteDetails(origin, destination);
}
