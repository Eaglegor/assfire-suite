#pragma once

#include <atomic>
#include <memory>

#include "assfire/router/api/RouterEngineType.hpp"
#include "assfire/router/api/DistanceMatrixCachingPolicy.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include <assfire/router/api/RoutingContext.hpp>
#include <assfire/router/api/DistanceMatrixErrorPolicy.hpp>
#include "transport/GrpcProtobufClient.hpp"

namespace assfire::router {
    class ClientDistanceMatrixFactory{
    public:
        ClientDistanceMatrixFactory(std::unique_ptr<ProtobufClient> grpc_client);

        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type, DistanceMatrixCachingPolicy caching_policy, const RoutingProfile &routing_profile, const RouteProviderSettings &settings,
                                            DistanceMatrixErrorPolicy error_policy = DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY) const;

    private:
        mutable std::atomic_int tag_counter;
        std::unique_ptr<ProtobufClient> grpc_client;
    };
}