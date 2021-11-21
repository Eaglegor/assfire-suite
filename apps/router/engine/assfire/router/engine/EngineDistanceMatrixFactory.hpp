#pragma once

#include "assfire/router/api/RouterEngineType.hpp"
#include "assfire/router/api/DistanceMatrixCachingPolicy.hpp"
#include "assfire/router/api/DistanceMatrixErrorPolicy.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include <assfire/router/api/RoutingContext.hpp>
#include <memory>
#include <atomic>

namespace assfire::router {
    class EngineDistanceMatrixFactory {
    public:
        EngineDistanceMatrixFactory(const RoutingContext& routing_context);

        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings,
                                            DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY) const;

    private:
        RoutingContext routing_context;
        mutable std::atomic_int tag_counter;
    };
}