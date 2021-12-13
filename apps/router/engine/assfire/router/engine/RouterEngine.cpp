#include "RouterEngine.hpp"

using namespace assfire::router;

RouterEngine::RouterEngine(RoutingContext routing_context) :
        distance_matrix_factory(routing_context) {
}

DistanceMatrix
RouterEngine::createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings,
                                   DistanceMatrixErrorPolicy error_policy) const {
    return distance_matrix_factory.createDistanceMatrix(engine_type, caching_policy, routing_profile, settings, error_policy);
}

RouteInfo RouterEngine::getRouteInfo(const DistanceMatrix::Location &origin, const DistanceMatrix::Location &destination, RouterEngineType engine_type, const RoutingProfile &routing_profile,
                                     const RouteProviderSettings &settings) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getRouteInfo(origin,
                                                                                                                                                                         destination);
}

RouteDetails RouterEngine::getRouteDetails(const DistanceMatrix::Location &origin, const DistanceMatrix::Location &destination, RouterEngineType engine_type, const RoutingProfile &routing_profile,
                                           const RouteProviderSettings &settings) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getRouteDetails(origin,
                                                                                                                                                                            destination);
}

TripInfo
RouterEngine::getTripInfo(const DistanceMatrixEngine::LocationsList &locations_list, RouterEngineType engine_type, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getTripInfo(locations_list);
}

TripDetails RouterEngine::getTripDetails(const DistanceMatrixEngine::LocationsList &locations_list, RouterEngineType engine_type, const RoutingProfile &routing_profile,
                                         const RouteProviderSettings &settings) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getTripDetails(locations_list);
}
