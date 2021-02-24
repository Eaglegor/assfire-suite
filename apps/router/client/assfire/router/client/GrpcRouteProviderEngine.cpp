#include "GrpcRouteProviderEngine.hpp"
#include <assfire/api/v1/service/router/translators/LocationTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RoutingOptionsTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RoutingProfileTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RouteInfoTranslator.hpp>


using namespace assfire::router;
using namespace assfire::router::proto_translation;

using GetSingleRouteRequest = ProtobufClient::GetSingleRouteRequest;
using GetSingleRouteResponse = ProtobufClient::GetSingleRouteResponse;
using GetRoutesBatchRequest = ProtobufClient::GetRoutesBatchRequest;
using GetRoutesBatchResponse = ProtobufClient::GetRoutesBatchResponse;
using ResponseStatus = ProtobufClient::ResponseStatus;

GrpcRouteProviderEngine::GrpcRouteProviderEngine(const ProtobufClient &client, RouterEngineType engineType, const RouteProviderSettings &settings, const RoutingProfile &routingProfile) :
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
    request.mutable_options()->set_retrieve_waypoints(false); // We don't need waypoints for sole RouteInfo

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

namespace {
    std::string buildLocationKey(const Location& location) {
        return std::to_string(location.getLatitude().encodedValue()) + std::to_string(location.getLongitude().encodedValue());
    }
}

Matrix<RouteInfo> GrpcRouteProviderEngine::getRouteInfoMatrix(const RouteProviderEngine::Locations &origins, const RouteProviderEngine::Locations &destinations) {
    GetRoutesBatchRequest request;

    int unique_locations_counter = 0;
    std::unordered_map<std::string, int> locations_mapping;

    for(const Location& location : origins) {
        request.add_origins()->CopyFrom(LocationTranslator::toProto(location));
        auto key = buildLocationKey(location);
        if(!locations_mapping.contains(key)) {
            locations_mapping.insert_or_assign(key, unique_locations_counter++);
        }
    }
    for(const Location& location : destinations) {
        request.add_destinations()->CopyFrom(LocationTranslator::toProto(location));
        auto key = buildLocationKey(location);
        if(!locations_mapping.contains(key)) {
            locations_mapping.insert_or_assign(key, unique_locations_counter++);
        }
    }

    request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
    request.mutable_options()->CopyFrom(RoutingOptionsTranslator::toProto(settings, engine_type));
    request.mutable_options()->set_retrieve_waypoints(false); // We don't need waypoints for sole RouteInfo

    GetRoutesBatchResponse response;
    client.getRoutesBatch(request, [&](const GetRoutesBatchResponse& reply) {
        response.CopyFrom(reply);
    });

    if(response.status().code() != ResponseStatus::RESPONSE_STATUS_CODE_OK){
        throw std::runtime_error(response.status().message());
    }

    Matrix<RouteInfo> result(origins.size(), destinations.size(), [&](int i, int j){
        return RouteInfo::infinity();
    });

    for(const auto& rinfo : response.route_infos()) {
        auto from_key = buildLocationKey(LocationTranslator::fromProto(rinfo.origin()));
        auto to_key = buildLocationKey(LocationTranslator::fromProto(rinfo.destination()));
        assert(locations_mapping.contains(from_key) && locations_mapping.contains(to_key));
        result.at(locations_mapping.at(from_key), locations_mapping.at(to_key)) = RouteInfoTranslator::fromProto(rinfo);
    }

    return result;
}

