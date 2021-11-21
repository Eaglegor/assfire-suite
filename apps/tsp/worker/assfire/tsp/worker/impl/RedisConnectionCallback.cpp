#include "RedisConnectionCallback.hpp"
#include <spdlog/spdlog.h>

#include <utility>

namespace assfire::tsp {
    RedisConnectionCallback::RedisConnectionCallback(std::string name) : name(std::move(name)) {}

    void RedisConnectionCallback::operator()(const std::string &host, std::size_t port, cpp_redis::client::connect_state status) {
        std::string string_status;
        switch (status) {
            case cpp_redis::client::connect_state::dropped:
                string_status = "DROPPED";
                break;
            case cpp_redis::client::connect_state::start:
                string_status = "START";
                break;
            case cpp_redis::client::connect_state::sleeping:
                string_status = "SLEEPING";
                break;
            case cpp_redis::client::connect_state::ok:
                string_status = "OK";
                break;
            case cpp_redis::client::connect_state::failed:
                string_status = "FAILED";
                break;
            case cpp_redis::client::connect_state::lookup_failed:
                string_status = "LOOKUP_FAILED";
                break;
            case cpp_redis::client::connect_state::stopped:
                string_status = "STOPPED";
                break;
            default:
                string_status = "UNKNOWN";
                break;
        }
        SPDLOG_INFO("Redis connection state for {}:{} ({}) has changed to {}", host, port, name, string_status);
    }
}