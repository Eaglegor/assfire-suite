#include "CppRedisCacheConnector.hpp"
#include <spdlog/spdlog.h>
#include <cpp_redis/core/reply.hpp>
#include <cpp_redis/core/client.hpp>
#include <future>

assfire::router::CppRedisCacheConnector::CppRedisCacheConnector(const std::string& redis_host, std::size_t redis_port) {
    SPDLOG_INFO("Creating redis client...");
    client = std::make_unique<cpp_redis::client>();
    SPDLOG_INFO("Redis client connecting to {}:{}...", redis_host, redis_port);
    client->connect(redis_host, redis_port);
}

void assfire::router::CppRedisCacheConnector::put(const std::string &key, const std::string &payload) {
    client->set(key, payload, [](const cpp_redis::reply &rpl) {
        if (rpl.is_error()) {
            SPDLOG_ERROR("Error while saving value to redis cache: {}", rpl.error());
        }
    });
    client->commit();
}

assfire::router::CacheConnector::CacheEntry assfire::router::CppRedisCacheConnector::get(const std::string &key) {
    std::future<cpp_redis::reply> freply = client->get(key);
    client->sync_commit();

    CacheEntry result;

    cpp_redis::reply reply = freply.get();
    if (reply.is_error()) {
        SPDLOG_ERROR("Error while retrieving value from redis cache: {}", reply.error());
        result.is_error = true;
        result.message = reply.error();
    } else if(reply.is_string()) {
        result.is_present = true;
        result.payload = reply.as_string();
    }

    return result;
}

assfire::router::CppRedisCacheConnector::~CppRedisCacheConnector() {

}


