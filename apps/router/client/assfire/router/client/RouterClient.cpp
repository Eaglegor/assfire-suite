#include "RouterClient.hpp"
#include "assfire/router/client/transport/GrpcProtobufClient.hpp"

#include <spdlog/spdlog.h>

assfire::router::RouterClient::RouterClient(const std::string &server_host, Port server_port, bool use_ssl)
        : transport_client(std::make_unique<GrpcProtobufClient>(server_host, server_port, use_ssl))
{
}

assfire::router::RouterClient::RouterClient(std::unique_ptr<GrpcProtobufClient> client)
        : transport_client(std::move(client))
{
}

assfire::router::RouterClient::~RouterClient() = default;

assfire::router::DistanceMatrix assfire::router::RouterClient::createDistanceMatrix(RouterEngineType engine_type,
                                                                                    DistanceMatrixCachingPolicy caching_policy,
                                                                                    const RoutingProfile &routing_profile,
                                                                                    const RouteProviderSettings &settings) const
{
    return distance_matrix_factory.createDistanceMatrix(engine_type, caching_policy, routing_profile, settings);
}

assfire::router::RouteInfo assfire::router::RouterClient::getRouteInfo(const Location &origin,
                                                                       const Location &destination,
                                                                       assfire::router::RouterEngineType engine_type,
                                                                       const assfire::router::RoutingProfile &routing_profile,
                                                                       const assfire::router::RouteProviderSettings &settings) const
{
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings).getRouteInfo(origin, destination);
}

assfire::router::RouteDetails assfire::router::RouterClient::getRouteDetails(const Location &origin,
                                                                             const Location &destination,
                                                                             assfire::router::RouterEngineType engine_type,
                                                                             const assfire::router::RoutingProfile &routing_profile,
                                                                             const assfire::router::RouteProviderSettings &settings) const
{
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings).getRouteDetails(origin, destination);
}
