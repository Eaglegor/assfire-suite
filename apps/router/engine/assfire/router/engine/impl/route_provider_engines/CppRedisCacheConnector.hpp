#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include "CacheConnector.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::router {
    class CppRedisCacheConnector : public CacheConnector {
    public:
        CppRedisCacheConnector(const std::string& redis_host, std::size_t redis_port);

        virtual ~CppRedisCacheConnector();

        void put(const std::string &key, const std::string &payload) override;

        CacheEntry get(const std::string &key) override;

    private:
        std::unique_ptr<cpp_redis::client> client;
    };
}