#pragma once

#include "EngineDistanceMatrixFactory.hpp"
#include "assfire/router/api/RouterApi.hpp"

namespace assfire::router
{
    class RouterEngine : public RouterApi {
    public:
        RouterEngine(RoutingContext routing_context);

        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type,
                                            DistanceMatrixCachingPolicy caching_policy,
                                            const RoutingProfile &routing_profile,
                                            const RouteProviderSettings &settings,
                                            DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY) const override;

        RouteInfo getRouteInfo(const DistanceMatrix::Location &origin,
                               const DistanceMatrix::Location &destination,
                               RouterEngineType engine_type,
                               const RoutingProfile &routing_profile,
                               const RouteProviderSettings &settings) const override;

        RouteDetails getRouteDetails(const DistanceMatrix::Location &origin,
                                     const DistanceMatrix::Location &destination,
                                     RouterEngineType engine_type,
                                     const RoutingProfile &routing_profile,
                                     const RouteProviderSettings &settings) const override;

        TripInfo getTripInfo(const DistanceMatrixEngine::LocationsList &locations_list, RouterEngineType engine_type, const RoutingProfile &routing_profile,
                             const RouteProviderSettings &settings) const override;

        TripDetails getTripDetails(const DistanceMatrixEngine::LocationsList &locations_list, RouterEngineType engine_type, const RoutingProfile &routing_profile,
                                   const RouteProviderSettings &settings) const override;

    private:
        EngineDistanceMatrixFactory distance_matrix_factory;
    };
}