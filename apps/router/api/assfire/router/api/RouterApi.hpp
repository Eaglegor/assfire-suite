#pragma once

#include "assfire/router/api/DistanceMatrixCachingPolicy.hpp"
#include "assfire/router/api/DistanceMatrixErrorPolicy.hpp"
#include "assfire/router/api/RouterEngineType.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/api/RouteInfo.hpp"
#include "assfire/router/api/RouteDetails.hpp"

namespace assfire::router {
    class RouterApi {
    public:
        virtual ~RouterApi() = default;

        virtual DistanceMatrix createDistanceMatrix(RouterEngineType engine_type,
                                                    DistanceMatrixCachingPolicy caching_policy,
                                                    const RoutingProfile &routing_profile,
                                                    const RouteProviderSettings &settings,
                                                    DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY) const = 0;

        virtual RouteInfo getRouteInfo(const Location &origin,
                                       const Location &destination,
                                       RouterEngineType engine_type,
                                       const RoutingProfile &routing_profile,
                                       const RouteProviderSettings &settings) const = 0;

        virtual RouteDetails getRouteDetails(const Location &origin,
                                             const Location &destination,
                                             RouterEngineType engine_type,
                                             const RoutingProfile &routing_profile,
                                             const RouteProviderSettings &settings) const = 0;
    };
}