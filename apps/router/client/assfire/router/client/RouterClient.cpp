#include "RouterClient.hpp"
#include "assfire/router/client/transport/GrpcProtobufClient.hpp"

#include <spdlog/spdlog.h>

assfire::router::RouterClient::RouterClient(const std::string &server_host, Port server_port, bool use_ssl)
        : distance_matrix_factory(std::make_unique<GrpcProtobufClient>(server_host, server_port, use_ssl)) {
}

assfire::router::RouterClient::RouterClient(std::unique_ptr<ProtobufClient> client)
        : distance_matrix_factory(std::move(client)) {
}

assfire::router::RouterClient::~RouterClient() = default;

assfire::router::DistanceMatrix assfire::router::RouterClient::createDistanceMatrix(RouterEngineType engine_type,
                                                                                    DistanceMatrixCachingPolicy caching_policy,
                                                                                    const RoutingProfile &routing_profile,
                                                                                    const RouteProviderSettings &settings,
                                                                                    DistanceMatrixErrorPolicy error_policy) const {
    return distance_matrix_factory.createDistanceMatrix(engine_type, caching_policy, routing_profile, settings, error_policy);
}

assfire::router::RouteInfo assfire::router::RouterClient::getRouteInfo(const DistanceMatrix::Location &origin,
                                                                       const DistanceMatrix::Location &destination,
                                                                       assfire::router::RouterEngineType engine_type,
                                                                       const assfire::router::RoutingProfile &routing_profile,
                                                                       const assfire::router::RouteProviderSettings &settings) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getRouteInfo(origin, destination);
}

assfire::router::RouteDetails assfire::router::RouterClient::getRouteDetails(const DistanceMatrix::Location &origin,
                                                                             const DistanceMatrix::Location &destination,
                                                                             assfire::router::RouterEngineType engine_type,
                                                                             const assfire::router::RoutingProfile &routing_profile,
                                                                             const assfire::router::RouteProviderSettings &settings) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getRouteDetails(origin, destination);
}

assfire::router::TripInfo assfire::router::RouterClient::getTripInfo(const assfire::router::DistanceMatrixEngine::LocationsList &locations_list, assfire::router::RouterEngineType engine_type,
                                                                     const assfire::router::RoutingProfile &routing_profile, const assfire::router::RouteProviderSettings &settings) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getTripInfo(locations_list);
}

assfire::router::TripDetails assfire::router::RouterClient::getTripDetails(const assfire::router::DistanceMatrixEngine::LocationsList &locations_list, assfire::router::RouterEngineType engine_type,
                                                                           const assfire::router::RoutingProfile &routing_profile, const assfire::router::RouteProviderSettings &settings) const {
    return createDistanceMatrix(engine_type, DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings, DistanceMatrixErrorPolicy::ON_ERROR_THROW).getTripDetails(locations_list);
}
