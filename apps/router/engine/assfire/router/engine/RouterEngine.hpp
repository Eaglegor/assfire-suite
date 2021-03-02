#pragma once

#include "DistanceMatrixFactory.hpp"

namespace assfire::router
{
    class RouterEngine {
    public:
        RouterEngine(RoutingContext routing_context);

        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type,
                                            DistanceMatrixCachingPolicy caching_policy,
                                            const RoutingProfile &routing_profile,
                                            const RouteProviderSettings &settings,
                                            DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY) const;

        RouteInfo getRouteInfo(const Location &origin,
                               const Location &destination,
                               RouterEngineType engine_type,
                               const RoutingProfile &routing_profile,
                               const RouteProviderSettings &settings) const;

        RouteDetails getRouteDetails(const Location &origin,
                                     const Location &destination,
                                     RouterEngineType engine_type,
                                     const RoutingProfile &routing_profile,
                                     const RouteProviderSettings &settings) const;

    private:
        RoutingContext routing_context;
        DistanceMatrixFactory distance_matrix_factory;
    };
}