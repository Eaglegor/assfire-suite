#pragma once

#include <cpp_redis/core/reply.hpp>
#include <cpp_redis/core/client.hpp>

namespace assfire::tsp {
    class RedisConnectionCallback {
    public:
        RedisConnectionCallback(std::string name);

        void operator()(const std::string &host, std::size_t port, cpp_redis::client::connect_state status);

    private:
        std::string name;
    };
}