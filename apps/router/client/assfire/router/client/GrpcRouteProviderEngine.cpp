#include "GrpcRouteProviderEngine.hpp"
#include <assfire/api/v1/service/router/translators/LocationTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RoutingOptionsTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RoutingProfileTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RouteInfoTranslator.hpp>


using namespace assfire::router;
using namespace assfire::router::proto_translation;

using GetSingleRouteRequest = GrpcClient::GetSingleRouteRequest;
using GetSingleRouteResponse = GrpcClient::GetSingleRouteResponse;
using GetRoutesBatchRequest = GrpcClient::GetRoutesBatchRequest;
using GetRoutesBatchResponse = GrpcClient::GetRoutesBatchResponse;
using ResponseStatus = GrpcClient::ResponseStatus;

GrpcRouteProviderEngine::GrpcRouteProviderEngine(const GrpcClient &client, RouterEngineType engineType, const RouteProviderSettings &settings, const RoutingProfile &routingProfile) :
        client(client),
        engine_type(engineType),
        settings(settings),
        routing_profile(routingProfile) {}


RouteInfo GrpcRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    GetSingleRouteRequest request;
    request.mutable_origin()->CopyFrom(LocationTranslator::toProto(origin));
    request.mutable_destination()->CopyFrom(LocationTranslator::toProto(destination));
    request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
    request.mutable_options()->CopyFrom(RoutingOptionsTranslator::toProto(settings, engine_type));

    GetSingleRouteResponse response = client.getRoute(request);

    if(response.status().code() != ResponseStatus::RESPONSE_STATUS_CODE_OK){
        throw std::runtime_error(response.status().message());
    }

    return RouteInfoTranslator::fromProto(response.route_info());
}

RouteDetails GrpcRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    GetSingleRouteRequest request;
    request.mutable_origin()->CopyFrom(LocationTranslator::toProto(origin));
    request.mutable_destination()->CopyFrom(LocationTranslator::toProto(destination));
    request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
    request.mutable_options()->CopyFrom(RoutingOptionsTranslator::toProto(settings, engine_type));

    GetSingleRouteResponse response = client.getRoute(request);

    if(response.status().code() != ResponseStatus::RESPONSE_STATUS_CODE_OK){
        throw std::runtime_error(response.status().message());
    }

    return RouteInfoTranslator::fromProtoWithDetails(response.route_info());
}

