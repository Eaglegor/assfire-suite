#include "GrpcRouteProviderEngine.hpp"
#include "assfire/api/v1/locations/translators/LocationTranslator.hpp"
#include <assfire/api/v1/router/translators/RouteProviderSettingsTranslator.hpp>
#include <assfire/api/v1/router/translators/RoutingProfileTranslator.hpp>
#include <assfire/api/v1/router/translators/RouteInfoTranslator.hpp>
#include <unordered_map>


using namespace assfire;
using namespace assfire::router;

using RouterEngineTypeTranslator = assfire::api::v1::router::RouterEngineTypeTranslator;
using RoutingProfileTranslator = assfire::api::v1::router::RoutingProfileTranslator;
using RouteProviderSettingsTranslator = assfire::api::v1::router::RouteProviderSettingsTranslator;
using LocationTranslator = assfire::api::v1::locations::LocationTranslator;
using RouteInfoTranslator = assfire::api::v1::router::RouteInfoTranslator;

using GetSingleRouteRequest = ProtobufClient::GetSingleRouteRequest;
using GetSingleRouteResponse = ProtobufClient::GetSingleRouteResponse;
using GetRoutesBatchRequest = ProtobufClient::GetRoutesBatchRequest;
using GetRoutesBatchResponse = ProtobufClient::GetRoutesBatchResponse;
using ResponseStatus = ProtobufClient::ResponseStatus;

GrpcRouteProviderEngine::GrpcRouteProviderEngine(const ProtobufClient &client, RouterEngineType engineType, const RouteProviderSettings &settings, const RoutingProfile &routingProfile)
        :
        client(client),
        engine_type(engineType),
        settings(settings),
        routing_profile(routingProfile) {}


RouteInfo GrpcRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    GetSingleRouteRequest request;
    request.mutable_origin()->CopyFrom(LocationTranslator::toProto(origin));
    request.mutable_destination()->CopyFrom(LocationTranslator::toProto(destination));
    request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
    request.mutable_settings()->CopyFrom(RouteProviderSettingsTranslator::toProto(settings, engine_type));
    request.mutable_settings()->set_retrieve_waypoints(false); // We don't need waypoints for sole RouteInfo

    GetSingleRouteResponse response = client.getRoute(request);

    if (response.status().code() != ResponseStatus::RESPONSE_STATUS_CODE_OK) {
        throw std::runtime_error(response.status().message());
    }

    return RouteInfoTranslator::fromProto(response.route_info());
}

RouteDetails GrpcRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    GetSingleRouteRequest request;
    request.mutable_origin()->CopyFrom(LocationTranslator::toProto(origin));
    request.mutable_destination()->CopyFrom(LocationTranslator::toProto(destination));
    request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
    request.mutable_settings()->CopyFrom(RouteProviderSettingsTranslator::toProto(settings, engine_type));

    GetSingleRouteResponse response = client.getRoute(request);

    if (response.status().code() != ResponseStatus::RESPONSE_STATUS_CODE_OK) {
        throw std::runtime_error(response.status().message());
    }

    return RouteInfoTranslator::fromProtoWithDetails(response.route_info());
}

namespace
{
    std::string buildLocationKey(const RouteProviderEngine::Location &location) {
        return std::to_string(location.getLatitude().encodedValue()) + " " + std::to_string(location.getLongitude().encodedValue());
    }
}

Matrix<RouteInfo> GrpcRouteProviderEngine::getRouteInfoMatrix(const RouteProviderEngine::LocationsList &origins, const RouteProviderEngine::LocationsList &destinations) const {
    GetRoutesBatchRequest request;

    std::unordered_multimap<std::string, int> origins_mapping;
    std::unordered_multimap<std::string, int> destinations_mapping;

    int origins_index = 0;
    for (const Location &location: origins) {
        auto key = buildLocationKey(location);
        if (!origins_mapping.contains(key)) {
            request.add_origins()->CopyFrom(LocationTranslator::toProto(location));
        }
        origins_mapping.emplace(key, origins_index++);
    }

    int destinations_index = 0;
    for (const Location &location: destinations) {
        auto key = buildLocationKey(location);
        if (!destinations_mapping.contains(key)) {
            request.add_destinations()->CopyFrom(LocationTranslator::toProto(location));
        }
        destinations_mapping.emplace(key, destinations_index++);
    }

    request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
    request.mutable_settings()->CopyFrom(RouteProviderSettingsTranslator::toProto(settings, engine_type));
    request.mutable_settings()->set_retrieve_waypoints(false); // We don't need waypoints for sole RouteInfo

    GetRoutesBatchResponse response;
    client.getRoutesBatch(request, [&](const GetRoutesBatchResponse &reply) {
        response.CopyFrom(reply);
    });

    if (response.status().code() != ResponseStatus::RESPONSE_STATUS_CODE_OK) {
        throw std::runtime_error(response.status().message());
    }

    Matrix<RouteInfo> result(origins.size(), destinations.size(), [&](int i, int j) {
        return RouteInfo::infinity();
    });

    for (const auto &rinfo: response.route_infos()) {
        auto from_key = buildLocationKey(LocationTranslator::fromProto(rinfo.origin()));
        auto to_key = buildLocationKey(LocationTranslator::fromProto(rinfo.destination()));
        assert(origins_mapping.contains(from_key) && destinations_mapping.contains(to_key));
        auto origins_range = origins_mapping.equal_range(from_key);
        auto destinations_range = destinations_mapping.equal_range(to_key);
        for (auto origin_iter = origins_range.first; origin_iter != origins_range.second; ++origin_iter) {
            for (auto destination_iter = destinations_range.first; destination_iter != destinations_range.second; ++destination_iter) {
                int from_id = origin_iter->second;
                int to_id = destination_iter->second;
                if (from_id == to_id) {
                    result.set(from_id, to_id, RouteInfo::zero());
                } else {
                    result.set(from_id, to_id, RouteInfoTranslator::fromProto(rinfo));
                }
            }
        }
    }

    return result;
}

Matrix<RouteDetails> GrpcRouteProviderEngine::getRouteDetailsMatrix(const RouteProviderEngine::LocationsList &origins, const RouteProviderEngine::LocationsList &destinations) const {
    GetRoutesBatchRequest request;

    std::unordered_multimap<std::string, int> origins_mapping;
    std::unordered_multimap<std::string, int> destinations_mapping;

    int origins_index = 0;
    for (const Location &location: origins) {
        auto key = buildLocationKey(location);
        if (!origins_mapping.contains(key)) {
            request.add_origins()->CopyFrom(LocationTranslator::toProto(location));
        }
        origins_mapping.emplace(key, origins_index++);
    }

    int destinations_index = 0;
    for (const Location &location: destinations) {
        auto key = buildLocationKey(location);
        if (!destinations_mapping.contains(key)) {
            request.add_destinations()->CopyFrom(LocationTranslator::toProto(location));
        }
        destinations_mapping.emplace(key, destinations_index++);
    }

    request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
    request.mutable_settings()->CopyFrom(RouteProviderSettingsTranslator::toProto(settings, engine_type));

    Matrix<RouteDetails> result(origins.size(), destinations.size(), [&](int i, int j) {
        return i == j ? RouteDetails::zero(origins[i], origins[j]) : RouteDetails::infinity(origins[i], origins[j]);
    });

    client.getRoutesBatch(request, [&](const GetRoutesBatchResponse &reply) {

        if (reply.status().code() != ResponseStatus::RESPONSE_STATUS_CODE_OK) {
            throw std::runtime_error(reply.status().message());
        }

        for (const auto &rinfo: reply.route_infos()) {
            Location origin = LocationTranslator::fromProto(rinfo.origin());
            Location destination = LocationTranslator::fromProto(rinfo.destination());

            auto from_key = buildLocationKey(origin);
            auto to_key = buildLocationKey(destination);
            assert(origins_mapping.contains(from_key) && destinations_mapping.contains(to_key));
            auto origins_range = origins_mapping.equal_range(from_key);
            auto destinations_range = destinations_mapping.equal_range(to_key);
            for (auto origin_iter = origins_range.first; origin_iter != origins_range.second; ++origin_iter) {
                for (auto destination_iter = destinations_range.first; destination_iter != destinations_range.second; ++destination_iter) {
                    int from_id = origin_iter->second;
                    int to_id = destination_iter->second;
                    if (from_id == to_id) {
                        result.set(from_id, to_id, RouteDetails::zero(origin, destination));
                    } else {
                        result.set(from_id, to_id, RouteInfoTranslator::fromProtoWithDetails(rinfo));
                    }
                }
            }
        }
    });

    return result;
}

