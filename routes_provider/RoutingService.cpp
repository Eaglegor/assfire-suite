#include "RoutingService.hpp"
#include "backends/redis/RedisProxy.hpp"
#include "backends/redis/TransparentProxy.hpp"
#include <spdlog/spdlog.h>
#include <random>

using namespace assfire;
using namespace assfire::routing::proto;

RoutingService::RoutingService(bool use_redis, const std::string& redis_host, std::size_t redis_port, RoutingMetricsCollector metrics_context):
	metrics_context(metrics_context),
	request_id_counter(0),
	euclidean_router(metrics_context),
	random_router(metrics_context)
{
	if (use_redis) {
		SPDLOG_INFO("Creating redis proxy...");
		caching_proxy = std::make_unique<RedisProxy>(redis_host, redis_port, metrics_context);
	}
	else {
		SPDLOG_INFO("Redis proxy is disabled. Using no-op proxy instead");
		caching_proxy = std::make_unique<TransparentProxy>();
	}
}

grpc::Status assfire::RoutingService::GetSingleRoute(grpc::ServerContext* context, const SingleRouteRequest* request, RouteInfo* response)
{
	metrics_context.addRequestedRoutes(1, RoutingMetricsCollector::RequestMode::SINGLE);
	metrics_context.routesRequestStopwatch(1, RoutingMetricsCollector::RequestMode::SINGLE);
	long request_id = request_id_counter++;
	SPDLOG_INFO("Single route request received, id = {}", request_id);
	try {
		response->CopyFrom(caching_proxy->getRoute(*request, selectRouter(request->options(), request_id), request_id));
	}
	catch (std::invalid_argument e) {
		return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
	}
	
	SPDLOG_INFO("Route request {} processing finished", request_id);
	return grpc::Status::OK;
}

grpc::Status assfire::RoutingService::GetRoutesBatch(grpc::ServerContext* context, const ManyToManyRoutesRequest* request, grpc::ServerWriter<RouteInfo>* stream)
{
	metrics_context.addRequestedRoutes(request->origins_size() * request->destinations_size(), RoutingMetricsCollector::RequestMode::BATCH);
	metrics_context.routesRequestStopwatch(request->origins_size() * request->destinations_size(), RoutingMetricsCollector::RequestMode::BATCH);
	long request_id = request_id_counter++;
	SPDLOG_INFO("Routes batch request received, id = {}", request_id);
	try {
		caching_proxy->getRoutesBatch(*request, [stream](RouteInfo result) {stream->Write(result); }, selectRouter(request->options(), request_id), request_id);
	}
	catch (std::invalid_argument e)	{
		return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
	}

	SPDLOG_INFO("Route request {} processing finished", request_id);
	return grpc::Status::OK;
}

grpc::Status assfire::RoutingService::GetStreamingRoutesBatch(grpc::ServerContext* context, grpc::ServerReaderWriter<RouteInfo, ManyToManyRoutesRequest>* stream)
{
	long request_id = request_id_counter++;
	SPDLOG_INFO("Streaming routes batch request received, id = {}", request_id);
	try {
		ManyToManyRoutesRequest request;
		while (stream->Read(&request)) {
			metrics_context.addRequestedRoutes(request.origins_size() * request.destinations_size(), RoutingMetricsCollector::RequestMode::STREAMING_BATCH);
			metrics_context.routesRequestStopwatch(request.origins_size() * request.destinations_size(), RoutingMetricsCollector::RequestMode::STREAMING_BATCH);
			caching_proxy->getRoutesBatch(request, [stream](RouteInfo result) {stream->Write(result); }, selectRouter(request.options(), request_id), request_id);
		}
	}
	catch (std::invalid_argument e)	{
		return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
	}

	SPDLOG_INFO("Route request {} processing finished", request_id);
	return grpc::Status::OK;
}

const Router& assfire::RoutingService::selectRouter(const RoutingOptions& options, long request_id) const
{
	switch (options.instance_case())
	{
	case RoutingOptions::InstanceCase::kEuclideanRoutingOptions:
		SPDLOG_DEBUG("Selecting euclidean router for request {}", request_id);
		return euclidean_router;
	case RoutingOptions::InstanceCase::kRandomRoutingOptions:
		SPDLOG_DEBUG("Selecting random router for request {}", request_id);
		return random_router;
	default:
		SPDLOG_ERROR("Supported router not found for request {}", request_id);
		throw std::invalid_argument("Requested router is unsupported");
	}
}
