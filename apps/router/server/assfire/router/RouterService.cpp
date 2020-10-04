#include "RouterService.hpp"
#include <spdlog/spdlog.h>
#include <assfire/router/providers/crowflight/CrowflightRouteProvider.hpp>
#include <assfire/router/providers/osrm/HttpOsrmRouteProvider.hpp>
#include <assfire/router/providers/euclidean/EuclideanRouteProvider.hpp>
#include <assfire/router/providers/random/RandomRouteProvider.hpp>
#include <assfire/router/cache/RedisRouteProviderProxy.hpp>
#include <assfire/router/CompositeRouteProvider.hpp>
#include <random>

using namespace assfire::router;

RouterService::RouterService(const Options &options) :
        request_id_counter(0)
{
    std::unique_ptr<CompositeRouteProvider> router = std::make_unique<CompositeRouteProvider>();
    router->addProvider(RouteProvider::RoutingOptions::CROWFLIGHT, std::make_unique<CrowflightRouteProvider>(options.metrics_collector));
    router->addProvider(RouteProvider::RoutingOptions::EUCLIDEAN, std::make_unique<EuclideanRouteProvider>(options.metrics_collector));
    router->addProvider(RouteProvider::RoutingOptions::RANDOM, std::make_unique<RandomRouteProvider>(options.metrics_collector));
    router->addProvider(RouteProvider::RoutingOptions::OSRM, std::make_unique<HttpOsrmRouteProvider>(options.osrm_endpoint, options.metrics_collector));

    if (options.use_redis) {
        SPDLOG_INFO("Using redis caching layer");
        route_provider = std::make_unique<RedisRouteProviderProxy>(std::move(router), options.redis_host, options.redis_port, options.metrics_collector);
    } else {
        SPDLOG_INFO("No caching layer is enabled");
        route_provider = std::move(router);
    }

    metrics_collector = options.metrics_collector;
}

grpc::Status RouterService::GetSingleRoute(grpc::ServerContext *context, const GetSingleRouteRequest *request, GetSingleRouteResponse *response)
{
    try {
        metrics_collector.addRequestedRoutes(1, MetricsCollector::RequestMode::SINGLE);
        metrics_collector.routesRequestStopwatch(1, MetricsCollector::RequestMode::SINGLE);
        long request_id = request_id_counter++;
        SPDLOG_INFO("Single route request received, id = {}", request_id);
        try {
            response->CopyFrom(route_provider->getRoute(*request, request_id));
        }
        catch (const std::invalid_argument &e) {
            SPDLOG_ERROR("Exception while processing single route request: {}", e.what());
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
        }

        SPDLOG_INFO("Route request {} processing finished", request_id);
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Error while processing single route request: {}", e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

grpc::Status RouterService::GetRoutesBatch(grpc::ServerContext *context, const GetRoutesBatchRequest *request, grpc::ServerWriter<GetRoutesBatchResponse> *stream)
{
    try {
        metrics_collector.addRequestedRoutes(request->origins_size() * request->destinations_size(),
                                             MetricsCollector::RequestMode::BATCH);
        metrics_collector.routesRequestStopwatch(request->origins_size() * request->destinations_size(),
                                                 MetricsCollector::RequestMode::BATCH);
        long request_id = request_id_counter++;
        SPDLOG_INFO("Routes batch request received, id = {}", request_id);
        try {
            route_provider->getRoutesBatch(*request, [stream](const GetRoutesBatchResponse result) { stream->Write(result); }, request_id);
        }
        catch (const std::invalid_argument &e) {
            SPDLOG_ERROR("Error while processing batch route request: {}", e.what());
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
        }

        SPDLOG_INFO("Route request {} processing finished", request_id);
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Error while processing batch route request: {}", e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

grpc::Status RouterService::GetStreamingRoutesBatch(grpc::ServerContext *context, grpc::ServerReaderWriter<GetRoutesBatchResponse, GetRoutesBatchRequest> *stream)
{
    try {
        long request_id = request_id_counter++;
        SPDLOG_INFO("Streaming routes batch request received, id = {}", request_id);
        try {
            GetRoutesBatchRequest request;
            while (stream->Read(&request)) {
                metrics_collector.addRequestedRoutes(request.origins_size() * request.destinations_size(),
                                                     MetricsCollector::RequestMode::STREAMING_BATCH);
                metrics_collector.routesRequestStopwatch(request.origins_size() * request.destinations_size(),
                                                         MetricsCollector::RequestMode::STREAMING_BATCH);
                route_provider->getRoutesBatch(request, [stream](const GetRoutesBatchResponse &result) { stream->Write(result); }, request_id);
            }
        }
        catch (const std::invalid_argument &e) {
            SPDLOG_ERROR("Error while processing streaming batch route request: {}", e.what());
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
        }

        SPDLOG_INFO("Route request {} processing finished", request_id);
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Error while processing streaming batch route request: {}", e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}
