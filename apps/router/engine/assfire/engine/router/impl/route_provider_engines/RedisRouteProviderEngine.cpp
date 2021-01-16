#include "RedisRouteProviderEngine.hpp"

#include <spdlog/spdlog.h>
#include <future>
#include <memory>
#include <cpp_redis/core/reply.hpp>
#include <cpp_redis/core/client.hpp>
#include <utility>

using namespace assfire::router;

RedisRouteProviderEngine::RedisRouteProviderEngine(const RoutingProfile &routingProfile, std::unique_ptr<RouteProviderSettings::Redis::Serializer> serializer, std::unique_ptr<RouteProviderEngine> backend_engine,
                                                   const std::string &redis_host,
                                                   std::size_t redis_port, bool force_update) :
        routing_profile(routingProfile),
        serializer(std::move(serializer)),
        force_update(force_update),
        backend_engine(std::move(backend_engine)) {
    SPDLOG_INFO("Creating redis client...");
    redis_client = std::make_unique<cpp_redis::client>();
    SPDLOG_INFO("Redis client connecting to {}:{}...", redis_host, redis_port);
    redis_client->connect(redis_host, redis_port);
}

RouteInfo RedisRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    return getSingleRouteDetails(origin, destination).getSummary();
}

RouteDetails RedisRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    SPDLOG_DEBUG("Requested route: ({},{})->({},{})",
                 origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                 destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue());

    std::string key = serializer->serializeKey(origin, destination);
    std::future<cpp_redis::reply> freply = redis_client->get(key);
    redis_client->sync_commit();

    cpp_redis::reply reply = freply.get();
    if (!reply.is_string() || reply.is_error() || force_update) {
        if (reply.is_error()) {
            SPDLOG_ERROR("Error while retrieving redis route: {}", reply.error());
        }
        if (!force_update) {
            SPDLOG_DEBUG("Route not found in cache: ({},{})->({},{}). Requesting backend service",
                         origin.getLatitude(), origin.getLongitude(),
                         destination.getLatitude(), destination.getLongitude());
        }

        RouteDetails result = backend_engine->getSingleRouteDetails(origin, destination);

        redis_client->set(key, serializer->serializeRouteDetails(origin, destination, result), [](const cpp_redis::reply &rpl) {
            if (rpl.is_error()) {
                SPDLOG_ERROR("Error while saving route to redis: {}", rpl.error());
            }
        });
        redis_client->commit();

        SPDLOG_DEBUG("Putting route to the cache ({},{})->({},{}) = (dist: {}, time: {})",
                     origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                     destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                     result.getSummary().getDistance(), result.getSummary().getDuration());
        return result;
    } else {
        RouteDetails result = serializer->deserializeRouteDetails(origin, destination, reply.as_string());
        SPDLOG_DEBUG("Route found in cache ({},{})->({},{}) = (dist: {}, time: {})",
                     origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                     destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                     result.getSummary().getDistance(), result.getSummary().getDuration());
        return result;
    }
}
