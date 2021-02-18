#pragma once

#include "assfire/api/router/RouterEngineType.hpp"
#include "assfire/api/router/DistanceMatrixCachingPolicy.hpp"
#include "assfire/api/router/model/RoutingProfile.hpp"
#include "assfire/api/router/DistanceMatrix.hpp"
#include "assfire/api/router/RouteProviderSettings.hpp"
#include <assfire/api/router/RoutingContext.hpp>
#include <atomic>
#include <memory>
#include "transport/GrpcClient.hpp"

namespace assfire::router {
    class ClientDistanceMatrixFactory {
    public:
        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) const;

    private:
        mutable std::atomic_int tag_counter;
        std::unique_ptr<GrpcClient> grpc_client;
    };
}