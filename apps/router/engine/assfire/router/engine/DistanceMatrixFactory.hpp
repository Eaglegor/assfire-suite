#pragma once

#include "assfire/router/api/RouterEngineType.hpp"
#include "assfire/router/api/DistanceMatrixCachingPolicy.hpp"
#include "assfire/router/api/DistanceMatrixErrorPolicy.hpp"
#include "assfire/router/api/model/RoutingProfile.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include <assfire/router/api/RoutingContext.hpp>
#include <atomic>

namespace assfire::router {
    class DistanceMatrixFactory {
    public:
        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings, const RoutingContext& context,
                                            DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY);

    private:
        std::atomic_int tag_counter;
    };
}