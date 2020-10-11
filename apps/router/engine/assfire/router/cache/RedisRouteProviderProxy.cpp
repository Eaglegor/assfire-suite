#include "RedisRouteProviderProxy.hpp"
#include <cpp_redis/core/client.hpp>

#include <assfire/router/CoordinatesDecoder.hpp>
#include <spdlog/spdlog.h>

using namespace assfire::router;

namespace {
    std::string buildRouteKey(const RouteProvider::Location &origin, const RouteProvider::Location &destination, const RouteProvider::RoutingOptions &options)
    {
        return origin.SerializeAsString() + destination.SerializeAsString() + options.SerializeAsString();
    }
}

RedisRouteProviderProxy::RedisRouteProviderProxy(std::unique_ptr<RouteProvider> &&backend, const std::string &host, std::size_t port, const MetricsCollector &metrics_collector) :
        backend(std::move(backend)),
        metrics_collector(metrics_collector)
{
    SPDLOG_INFO("Creating redis client...");
    redis_client = std::make_unique<cpp_redis::client>();
    SPDLOG_INFO("Redis client connecting to {}:{}...", host, port);
    redis_client->connect(host, port);
}

RouteProvider::GetSingleRouteResponse RedisRouteProviderProxy::getRoute(GetSingleRouteRequest request, long request_id) const
{
    metrics_collector.routesCacheStopwatch(1, MetricsCollector::RequestMode::SINGLE);
    metrics_collector.addRoutesCacheRequests(1, MetricsCollector::RequestMode::SINGLE);
    SPDLOG_DEBUG("[{}]: Requested route: ({},{})->({},{})", request_id,
                 request.origin().lat(), request.origin().lon(),
                 request.destination().lat(), request.destination().lon());

    std::string key = buildRouteKey(request.origin(), request.destination(), request.options());
    std::future<cpp_redis::reply> freply = redis_client->get(key);
    redis_client->sync_commit();

    RouteInfo route;

    cpp_redis::reply reply = freply.get();
    if (!reply.is_string() || reply.is_error() || request.options().force_update()) {
        if (reply.is_error()) {
            SPDLOG_ERROR("[{}]: Error while retrieving redis route: {}", request_id, reply.error());
        }
        if (!request.options().force_update()) {
            metrics_collector.addRoutesCacheMisses(1);
            SPDLOG_DEBUG("[{}]: Route not found in cache: ({},{})->({},{}). Requesting backend service", request_id,
                         request.origin().lat(), request.origin().lon(),
                         request.destination().lat(), request.destination().lon());
        }
        GetSingleRouteResponse routeResponse = backend->getRoute(request, request_id);
        if (routeResponse.status().code() != ResponseStatus::RESPONSE_STATUS_CODE_OK) {
            SPDLOG_ERROR("[{}]: Backend returned response with error: {}", routeResponse.status().message());
            return routeResponse;
        }
        route = routeResponse.route_info();
        redis_client->set(key, route.SerializeAsString(), [request_id](const cpp_redis::reply &rpl) {
            if (rpl.is_error()) {
                SPDLOG_ERROR("[{}]: Error while saving route to redis: {}", request_id, rpl.error());
            }
        });
        redis_client->commit();

        SPDLOG_DEBUG("[{}]: Putting route to the cache ({},{})->({},{}) = (dist: {}, time: {})", request_id,
                     request.origin().lat(), request.origin().lon(),
                     request.destination().lat(), request.destination().lon(),
                     route.distance(), route.duration());
    } else {
        metrics_collector.addRoutesCacheHits(1);
        route.ParseFromString(reply.as_string());
        SPDLOG_DEBUG("[{}]: Route found in cache ({},{})->({},{}) = (dist: {}, time: {})", request_id,
                     request.origin().lat(), request.origin().lon(),
                     request.destination().lat(), request.destination().lon(),
                     route.distance(), route.duration());
    }

    GetSingleRouteResponse response;
    response.mutable_route_info()->CopyFrom(route);
    return response;
}

RouteProvider::GetRoutesBatchResponse RedisRouteProviderProxy::getRoutesBatch(GetRoutesBatchRequest request, long request_id) const
{
    metrics_collector.routesCacheStopwatch(1, MetricsCollector::RequestMode::BATCH);
    metrics_collector.addRoutesCacheRequests(request.origins_size() * request.destinations_size(),
                                             MetricsCollector::RequestMode::BATCH);
    return calculateBatchUsingSingleRoutes(request, request_id);
}

void RedisRouteProviderProxy::getRoutesBatch(GetRoutesBatchRequest request, RoutesBatchConsumer consumer, long request_id) const
{
    metrics_collector.routesCacheStopwatch(1, MetricsCollector::RequestMode::STREAMING_BATCH);
    metrics_collector.addRoutesCacheRequests(request.origins_size() * request.destinations_size(),
                                             MetricsCollector::RequestMode::STREAMING_BATCH);
    consumer(calculateBatchUsingSingleRoutes(request, request_id));
}

RedisRouteProviderProxy::~RedisRouteProviderProxy() = default;

