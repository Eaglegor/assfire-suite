#pragma once

#include "RouterEngineType.hpp"
#include "DistanceMatrixCachingPolicy.hpp"
#include "RoutingProfile.hpp"
#include "DistanceMatrix.hpp"
#include "RouteProviderSettings.hpp"
#include <atomic>

namespace assfire::router
{
    class DistanceMatrixFactory
    {
    public:
        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings);

    private:
        std::atomic_int tag_counter;
    };
}