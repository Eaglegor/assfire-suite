#include "RouterService.hpp"
#include <spdlog/spdlog.h>
#include <assfire/api/v1/router/translators/Translators.hpp>
#include <assfire/api/v1/concepts/translators/Translators.hpp>
#include "assfire/router/engine/DefaultRedisSerializer.hpp"
#include <random>

using namespace assfire;
using namespace assfire::router;

namespace {
    constexpr const char* UNIDENTIFIED_REQUEST_ID = "?";
}

RouterService::RouterService(const Options &options) {
    RoutingContext routing_context;
    routing_context.getOsrmContext().setEndpoint(options.osrm_endpoint);
    routing_context.getRedisContext().setCacheEnabled(options.use_redis);
    routing_context.getRedisContext().setHost(options.redis_host);
    routing_context.getRedisContext().setPort(options.redis_port);
    routing_context.getRedisContext().setSerializerSupplier([](RouterEngineType engine_type, const RoutingProfile &profile, const RouteProviderSettings &settings) {
        return std::make_unique<DefaultRedisSerializer>(engine_type, profile, settings);
    });

    router_engine = std::make_unique<RouterEngine>(std::move(routing_context));
}

grpc::Status RouterService::GetSingleRoute(grpc::ServerContext *context, const GetSingleRouteRequest *request, GetSingleRouteResponse *response) {
    try {
        const auto &request_id = UNIDENTIFIED_REQUEST_ID;
        SPDLOG_INFO("Single route request received, id = {}", request_id);
        DistanceMatrix distance_matrix = router_engine->createDistanceMatrix(
                RouterEngineTypeTranslator::fromProto(request->settings().router_engine_type()),
                DistanceMatrixCachingPolicy::NO_CACHING,
                RoutingProfileTranslator::fromProto(request->routing_profile()),
                RouteProviderSettingsTranslator::fromProto(request->settings())
        );

        RouteDetails route_details = distance_matrix.getRouteDetails(LocationTranslator::fromProto(request->origin()), LocationTranslator::fromProto(request->destination()));

        response->mutable_status()->set_code(api::v1::router::ResponseStatus::RESPONSE_STATUS_CODE_OK);
        response->mutable_route_info()->CopyFrom(RouteInfoTranslator::toProto(request->origin(), request->destination(), route_details));

        SPDLOG_INFO("Route request {} processing finished", request_id);
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Error while processing single route request: {}", e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

grpc::Status RouterService::GetRoutesBatch(grpc::ServerContext *context, const GetRoutesBatchRequest *request, grpc::ServerWriter<GetRoutesBatchResponse> *stream) {
    try {
        const auto &request_id = UNIDENTIFIED_REQUEST_ID;
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
            const auto &request_id = UNIDENTIFIED_REQUEST_ID;

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
    DistanceMatrix distance_matrix = router_engine->createDistanceMatrix(
            RouterEngineTypeTranslator::fromProto(request.settings().router_engine_type()),
            DistanceMatrixCachingPolicy::AUTO,
            RoutingProfileTranslator::fromProto(request.routing_profile()),
            RouteProviderSettingsTranslator::fromProto(request.settings())
    );

    std::vector<IndexedLocation> indexed_origins;
    std::vector<IndexedLocation> indexed_destinations;

    indexed_origins.reserve(request.origins().size());
    indexed_destinations.reserve(request.destinations().size());

    for (const auto &from_loc : request.origins()) {
        indexed_origins.push_back(distance_matrix.addLocation(LocationTranslator::fromProto(from_loc), DistanceMatrixEngine::LocationType::ORIGIN));
    }

    for (const auto &to_loc : request.destinations()) {
        indexed_destinations.push_back(distance_matrix.addLocation(LocationTranslator::fromProto(to_loc), DistanceMatrixEngine::LocationType::DESTINATION));
    }

    for (const auto &from_loc : indexed_origins) {
        for (const auto &to_loc : indexed_destinations) {
            RouteDetails route_details = distance_matrix.getRouteDetails(from_loc, to_loc);

            if (from_loc == to_loc) {
                continue;
            }

            GetRoutesBatchResponse response;

            response.mutable_status()->set_code(api::v1::router::ResponseStatus::RESPONSE_STATUS_CODE_OK);
            api::v1::router::RouteInfo *route_info = response.add_route_infos();
            route_info->CopyFrom(RouteInfoTranslator::toProto(from_loc.getRawLocation(), to_loc.getRawLocation(), route_details));

            consumeResponse(response);
        }
    }


}


RouterService::~RouterService() = default;
