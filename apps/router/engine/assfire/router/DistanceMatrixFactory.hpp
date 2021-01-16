#pragma once

#include "RouterEngineType.hpp"
#include "DistanceMatrixCachingPolicy.hpp"

namespace assfire::router
{
    class DistanceMatrixFactory
    {
    public:
        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile)
        {

        }
    };
}