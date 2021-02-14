#include "RouterService.hpp"
#include <spdlog/spdlog.h>
#include <assfire/api/v1/service/router/translators/RoutingEngineTypeTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RoutingProfileTranslator.hpp>
#include <assfire/api/v1/service/router/translators/RoutingOptionsTranslator.hpp>
#include <assfire/api/v1/service/router/translators/LocationTranslator.hpp>
#include "assfire/engine/router/DefaultRedisSerializer.hpp"
#include <random>

using namespace assfire::router;
using namespace assfire::router::proto_translation;

namespace {
    constexpr const char* UNIDENTIFIED_REQUEST_ID = "?";
}

RouterService::RouterService(const Options &options) :
        metrics_collector(options.metrics_collector) {
    routing_context.getOsrmContext().setEndpoint(options.osrm_endpoint);
    routing_context.getRedisContext().setCacheEnabled(options.use_redis);
    routing_context.getRedisContext().setHost(options.redis_host);
    routing_context.getRedisContext().setPort(options.redis_port);
    routing_context.getRedisContext().setSerializerSupplier([](RouterEngineType engine_type, const RoutingProfile &profile, const RouteProviderSettings &settings) {
        return std::make_unique<DefaultRedisSerializer>(engine_type, profile, settings);
    });
}

grpc::Status RouterService::GetSingleRoute(grpc::ServerContext *context, const GetSingleRouteRequest *request, GetSingleRouteResponse *response) {
    try {
        const auto &request_id = !request->request_id().empty() ? request->request_id() : UNIDENTIFIED_REQUEST_ID;
        SPDLOG_INFO("Single route request received, id = {}", request_id);
        DistanceMatrix distance_matrix = distance_matrix_factory.createDistanceMatrix(
                RoutingEngineTypeTranslator::fromProto(request->options().routing_type()),
                DistanceMatrixCachingPolicy::NO_CACHING,
                RoutingProfileTranslator::fromProto(request->routing_profile()),
                RoutingOptionsTranslator::fromProto(request->options()),
                routing_context
        );

        RouteDetails route_details = distance_matrix.getRouteDetails(LocationTranslator::fromProto(request->origin()), LocationTranslator::fromProto(request->destination()));

        response->mutable_status()->set_code(api::v1::service::router::ResponseStatus::RESPONSE_STATUS_CODE_OK);
        response->mutable_route_info()->mutable_origin()->CopyFrom(request->origin());
        response->mutable_route_info()->mutable_destination()->CopyFrom(request->destination());
        response->mutable_route_info()->set_duration(route_details.getSummary().getDuration());
        response->mutable_route_info()->set_distance(route_details.getSummary().getDistance());

        if (request->options().retrieve_waypoints()) {
            for (const RouteDetails::Waypoint &waypoint : route_details.getWaypoints()) {
                auto *wp = response->mutable_route_info()->add_waypoints();
                wp->set_lat(waypoint.getLatitude().longValue());
                wp->set_lon(waypoint.getLongitude().longValue());
            }
        }
        SPDLOG_INFO("Route request {} processing finished", request_id);
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Error while processing single route request: {}", e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

grpc::Status RouterService::GetRoutesBatch(grpc::ServerContext *context, const GetRoutesBatchRequest *request, grpc::ServerWriter<GetRoutesBatchResponse> *stream) {
    try {
        const auto &request_id = !request->request_id().empty() ? request->request_id() : UNIDENTIFIED_REQUEST_ID;
        SPDLOG_INFO("Routes batch request received, id = {}", request_id);

        processBatchRequest(*request, [&](const auto &response) { stream->Write(response); });

        SPDLOG_INFO("Route request {} processing finished", request_id);
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Error while processing batch route request: {}", e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

grpc::Status RouterService::GetStreamingRoutesBatch(grpc::ServerContext *context, grpc::ServerReaderWriter<GetRoutesBatchResponse, GetRoutesBatchRequest> *stream) {
    try {
        SPDLOG_INFO("Streaming routes batch request initiated");
        GetRoutesBatchRequest request;
        while (stream->Read(&request)) {
            const auto &request_id = !request.request_id().empty() ? request.request_id() : UNIDENTIFIED_REQUEST_ID;

            SPDLOG_INFO("Streaming routes batch request received, id = {}", request_id);

            processBatchRequest(request, [&](const auto &response) { stream->Write(response); });

            SPDLOG_INFO("Streaming routes batch request {} processed", request_id);
        }
        SPDLOG_INFO("Streaming routes batch request processing finished");
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Error while processing streaming batch route request: {}", e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

void RouterService::processBatchRequest(const RouterService::GetRoutesBatchRequest &request, const std::function<void(const GetRoutesBatchResponse &)> &consumeResponse) {
    DistanceMatrix distance_matrix = distance_matrix_factory.createDistanceMatrix(
            RoutingEngineTypeTranslator::fromProto(request.options().routing_type()),
            DistanceMatrixCachingPolicy::NO_CACHING,
            RoutingProfileTranslator::fromProto(request.routing_profile()),
            RoutingOptionsTranslator::fromProto(request.options()),
            routing_context
    );

    for (const auto &from_loc : request.origins()) {
        for (const auto &to_loc : request.destinations()) {
            RouteDetails route_details = distance_matrix.getRouteDetails(LocationTranslator::fromProto(from_loc), LocationTranslator::fromProto(to_loc));

            if (from_loc.lat() == to_loc.lat() && from_loc.lon() == to_loc.lon()) {
                continue;
            }

            GetRoutesBatchResponse response;

            response.mutable_status()->set_code(api::v1::service::router::ResponseStatus::RESPONSE_STATUS_CODE_OK);
            api::v1::model::routing::RouteInfo *route_info = response.add_route_infos();
            route_info->mutable_origin()->CopyFrom(from_loc);
            route_info->mutable_destination()->CopyFrom(to_loc);
            route_info->set_duration(route_details.getSummary().getDuration());
            route_info->set_distance(route_details.getSummary().getDistance());

            if (request.options().retrieve_waypoints()) {
                for (const RouteDetails::Waypoint &waypoint : route_details.getWaypoints()) {
                    auto *wp = route_info->add_waypoints();
                    wp->set_lat(waypoint.getLatitude().longValue());
                    wp->set_lon(waypoint.getLongitude().longValue());
                }
            }

            consumeResponse(response);
        }
    }


}


RouterService::~RouterService() = default;
