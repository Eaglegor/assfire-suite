#include "RedisRouteProviderEngine.hpp"

#include <spdlog/spdlog.h>
#include <future>
#include <memory>
#include "CacheConnector.hpp"
#include <utility>

using namespace assfire::router;

RedisRouteProviderEngine::RedisRouteProviderEngine(const RoutingProfile &routing_profile,
                                                   std::unique_ptr<RedisSerializer> serializer,
                                                   std::unique_ptr<RouteProviderEngine> backend_engine,
                                                   std::unique_ptr<CacheConnector> redis_connector,
                                                   bool force_update,
                                                   EngineMetricsCollector metrics_collector)
        : routing_profile(routing_profile),
          redis_client(std::move(redis_connector)),
          serializer(std::move(serializer)),
          force_update(force_update),
          backend_engine(std::move(backend_engine)),
          metrics_collector(std::move(metrics_collector))
{
}

RouteInfo RedisRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const
{
    metrics_collector.recordSingleRouteInfoRetrievalFromRedis();
    return retrieveSingleRouteDetails(origin, destination).getSummary();
}

RouteDetails RedisRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const
{
    metrics_collector.recordSingleRouteDetailsRetrievalFromRedis();
    return retrieveSingleRouteDetails(origin, destination);
}

RouteDetails RedisRouteProviderEngine::retrieveSingleRouteDetails(const assfire::Location &origin, const assfire::Location &destination) const
{
    SPDLOG_DEBUG("Requested route: ({},{})->({},{})",
                 origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                 destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue());

    if (origin == destination) {
        SPDLOG_DEBUG("Returning empty route for: ({},{})->({},{})",
                     origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                     destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue());
        return RouteDetails(RouteInfo::zero(), {origin, destination});
    }

    auto stopwatch = metrics_collector.measureSingleRouteDetailsRetrievalFromRedis();

    std::string key = serializer->serializeKey(origin, destination);
    CacheConnector::CacheEntry reply = redis_client->get(key);

    if (reply.is_error || !reply.is_present || force_update) {
        if (!force_update) {
            metrics_collector.recordRedisCacheMiss();
            SPDLOG_DEBUG("Route not found in cache: ({},{})->({},{}). Requesting backend service",
                         origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                         destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue());
        }

        RouteDetails result = backend_engine->getSingleRouteDetails(origin, destination);

        redis_client->put(key, serializer->serializeRouteDetails(origin, destination, result));

        SPDLOG_DEBUG("Putting route to the cache ({},{})->({},{}) = (dist: {}, time: {})",
                     origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                     destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                     result.getSummary().getDistance().toMeters(), result.getSummary().getDuration().toSeconds());
        return result;
    } else {
        metrics_collector.recordRedisCacheHit();
        RouteDetails result = serializer->deserializeRouteDetails(origin, destination, reply.payload);
        SPDLOG_DEBUG("Route found in cache ({},{})->({},{}) = (dist: {}, time: {})",
                     origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                     destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                     result.getSummary().getDistance().toMeters(), result.getSummary().getDuration().toSeconds());
        return result;
    }
}
