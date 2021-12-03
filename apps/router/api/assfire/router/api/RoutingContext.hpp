#pragma once

#include <string>
#include <assfire/router/api/RouteDetails.hpp>
#include "RedisSerializer.hpp"

namespace assfire::router {
    class RoutingContext {
    public:
        class RedisContext {
        public:
            using SerializerSupplier = std::function<std::unique_ptr<RedisSerializer>(RouterEngineType, const RoutingProfile &, const RouteProviderSettings&)>;

            const std::string &getHost() const {
                return host;
            }

            void setHost(const std::string &host) {
                this->host = host;
            }

            size_t getPort() const {
                return port;
            }

            void setPort(size_t port) {
                this->port = port;
            }

            const SerializerSupplier &getSerializerSupplier() const {
                return serializer_supplier;
            }

            void setSerializerSupplier(const SerializerSupplier &serializerSupplier) {
                this->serializer_supplier = serializerSupplier;
            }

            bool isCacheEnabled() const {
                return cache_enabled;
            }

            void setCacheEnabled(bool cacheEnabled) {
                cache_enabled = cacheEnabled;
            }

        private:
            std::string host = "localhost";
            std::size_t port = 6379;
            bool cache_enabled = false;
            SerializerSupplier serializer_supplier;
        };

        class OsrmContext {
        public:
            const std::string &getEndpoint() const {
                return endpoint;
            }

            void setEndpoint(const std::string &endpoint) {
                OsrmContext::endpoint = endpoint;
            }

        private:
            std::string endpoint = "localhost:5000";
        };

        const OsrmContext &getOsrmContext() const {
            return osrm_context;
        }

        OsrmContext &getOsrmContext() {
            return osrm_context;
        }

        void setOsrmContext(const OsrmContext &osrm_context) {
            this->osrm_context = osrm_context;
        }

        const RedisContext &getRedisContext() const {
            return redis_context;
        }

        RedisContext &getRedisContext() {
            return redis_context;
        }

    private:
        OsrmContext osrm_context;
        RedisContext redis_context;
    };
}