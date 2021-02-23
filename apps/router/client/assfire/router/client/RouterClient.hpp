#pragma once

#include <memory>
#include <string>
#include <assfire/api/router/DistanceMatrix.hpp>
#include <assfire/api/router/DistanceMatrixCachingPolicy.hpp>
#include <assfire/api/router/RouterEngineType.hpp>
#include <assfire/api/router/model/RoutingProfile.hpp>
#include <assfire/api/router/RouteProviderSettings.hpp>
#include "ClientDistanceMatrixFactory.hpp"

namespace assfire::router {
    class GrpcProtobufClient;

    class RouterClient final {
    public:
        using Port = std::size_t;

        RouterClient(const std::string &server_host, Port server_port, bool use_ssl);
        RouterClient(std::unique_ptr<GrpcProtobufClient> client);

        ~RouterClient();

        DistanceMatrix createDistanceMatrix(RouterEngineType engine_type,
                                            DistanceMatrixCachingPolicy caching_policy,
                                            const RoutingProfile &routing_profile,
                                            const RouteProviderSettings &settings) const;

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
        std::unique_ptr<GrpcProtobufClient> transport_client;
        ClientDistanceMatrixFactory distance_matrix_factory;
    };
}