#pragma once

#include <memory>
#include <string>
#include <assfire/router/api/DistanceMatrix.hpp>
#include <assfire/router/api/DistanceMatrixCachingPolicy.hpp>
#include <assfire/router/api/RouterEngineType.hpp>
#include <assfire/router/api/RoutingProfile.hpp>
#include <assfire/router/api/RouteProviderSettings.hpp>
#include "ClientDistanceMatrixFactory.hpp"
#include "assfire/router/api/RouterApi.hpp"

namespace assfire::router {
    class ProtobufClient;

    class RouterClient : public RouterApi {
    public:
        using Port = std::size_t;

        RouterClient(const std::string &server_host, Port server_port, bool use_ssl);

        RouterClient(std::unique_ptr<ProtobufClient> client);

        ~RouterClient();

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
        ClientDistanceMatrixFactory distance_matrix_factory;
    };
}