#pragma once

#include "assfire/api/router/RouterEngineType.hpp"
#include "assfire/api/router/DistanceMatrixCachingPolicy.hpp"
#include "assfire/api/router/DistanceMatrixErrorPolicy.hpp"
#include "assfire/api/router/model/RoutingProfile.hpp"
#include "assfire/api/router/DistanceMatrix.hpp"
#include "assfire/api/router/RouteProviderSettings.hpp"
#include <assfire/api/router/RoutingContext.hpp>
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