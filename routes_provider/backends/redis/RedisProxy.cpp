#include "RedisProxy.hpp"
#include <cpp_redis/core/client.hpp>

#include <CoordinatesDecoder.hpp>
#include <spdlog/spdlog.h>

using namespace assfire;
using namespace assfire::routing::proto::v1;

namespace {
    std::string buildRouteKey(const Location &origin, const Location &destination, const RoutingOptions &options)
    {
        return origin.SerializeAsString() + destination.SerializeAsString() + options.SerializeAsString();
    }
}

RedisProxy::RedisProxy(const std::string &host, std::size_t port, RoutingMetricsCollector routing_metrics_context) :
        metrics_context(routing_metrics_context)
{
    SPDLOG_INFO("Creating redis client...");
    redis_client = std::make_unique<cpp_redis::client>();
    SPDLOG_INFO("Redis client connecting to {}:{}...", host, port);
    redis_client->connect(host, port);
}

GetSingleRouteResponse RedisProxy::getRoute(GetSingleRouteRequest request, const Router &backend, long request_id) const
{
    metrics_context.routesCacheStopwatch(1, RoutingMetricsCollector::RequestMode::SINGLE);
    metrics_context.addRoutesCacheRequests(1, RoutingMetricsCollector::RequestMode::SINGLE);
    SPDLOG_DEBUG("[{}]: Requested route: ({},{})->({},{})", request_id,
                 request.from().latitude(), request.from().longitude(),
                 request.to().latitude(), request.to().longitude());

    std::string key = buildRouteKey(request.from(), request.to(), request.options());
    std::future<cpp_redis::reply> freply = redis_client->get(key);
    redis_client->sync_commit();

    RouteInfo route;

    cpp_redis::reply reply = freply.get();
    if (!reply.is_string() || reply.is_error() || request.options().force_update()) {
        if (reply.is_error()) {
            SPDLOG_ERROR("[{}]: Error while retrieving redis route: {}", request_id, reply.error());
        }
        if (!request.options().force_update()) {
            metrics_context.addRoutesCacheMisses(1);
            SPDLOG_DEBUG("[{}]: Route not found in cache: ({},{})->({},{}). Requesting backend service", request_id,
                         request.from().latitude(), request.from().longitude(),
                         request.to().latitude(), request.to().longitude());
        }
        route = backend.getRoute(request, request_id).route_info();
        redis_client->set(key, route.SerializeAsString(), [request_id](const cpp_redis::reply &rpl) {
            if (rpl.is_error()) {
                SPDLOG_ERROR("[{}]: Error while saving route to redis: {}", request_id, rpl.error());
            }
        });
        redis_client->commit();

        SPDLOG_DEBUG("[{}]: Putting route to the cache ({},{})->({},{}) = (dist: {}, time: {})", request_id,
                     request.from().latitude(), request.from().longitude(),
                     request.to().latitude(), request.to().longitude(),
                     route.distance(), route.duration());
    } else {
        metrics_context.addRoutesCacheHits(1);
        route.ParseFromString(reply.as_string());
        SPDLOG_DEBUG("[{}]: Route found in cache ({},{})->({},{}) = (dist: {}, time: {})", request_id,
                     request.from().latitude(), request.from().longitude(),
                     request.to().latitude(), request.to().longitude(),
                     route.distance(), route.duration());
    }

    GetSingleRouteResponse response;
    response.mutable_route_info()->CopyFrom(route);
    return response;
}

GetRoutesBatchResponse
RedisProxy::getRoutesBatch(GetRoutesBatchRequest request, const Router &backend, long request_id) const
{
    metrics_context.routesCacheStopwatch(1, RoutingMetricsCollector::RequestMode::BATCH);
    metrics_context.addRoutesCacheRequests(request.origins_size() * request.destinations_size(),
                                           RoutingMetricsCollector::RequestMode::BATCH);
    return calculateBatchUsingSingleRoutes(request, backend, request_id);
}

void
RedisProxy::getRoutesBatch(GetRoutesBatchRequest request, Router::RoutesBatchConsumer consumer, const Router &backend,
                           long request_id) const
{
    metrics_context.routesCacheStopwatch(1, RoutingMetricsCollector::RequestMode::STREAMING_BATCH);
    metrics_context.addRoutesCacheRequests(request.origins_size() * request.destinations_size(),
                                           RoutingMetricsCollector::RequestMode::STREAMING_BATCH);
    consumer(calculateBatchUsingSingleRoutes(request, backend, request_id));
}

GetRoutesBatchResponse
RedisProxy::calculateBatchUsingSingleRoutes(GetRoutesBatchRequest request, const Router &backend, long request_id) const
{
    routing::proto::v1::GetRoutesBatchResponse response;
    for (const Location &origin : request.origins()) {
        for (const Location &destination : request.destinations()) {
            GetSingleRouteRequest single_request;
            single_request.mutable_from()->CopyFrom(origin);
            single_request.mutable_to()->CopyFrom(destination);
            single_request.mutable_options()->CopyFrom(request.options());
            response.add_route_infos()->CopyFrom(getRoute(single_request, backend, request_id));
        }
    }
    return response;
}

RedisProxy::~RedisProxy()
{
}
