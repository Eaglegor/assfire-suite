#pragma once

#include <memory>
#include <optional>
#include <string>

namespace assfire::router {
    class CacheConnector {
    public:
        struct CacheEntry {
            std::string payload;
            bool is_present = false;
            bool is_error = false;
            std::string message;
        };

        virtual ~CacheConnector() = default;

        virtual void put(const std::string& key, const std::string& payload) = 0;
        virtual CacheEntry get(const std::string& key) = 0;
    };
}