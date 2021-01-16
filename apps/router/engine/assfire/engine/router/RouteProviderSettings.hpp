#pragma once

#include <string>
#include <functional>

namespace assfire::router {
    class RouteProviderSettings {
    public:
        class Osrm {
        public:
            enum class Geometry {
                STRAIGHT_LINE,
                SIMPLIFIED,
                FULL
            };

            const std::string &getEndpoint() const {
                return endpoint;
            }

            Geometry getGeometry() const {
                return geometry;
            }

            void setEndpoint(const std::string &endpoint) {
                Osrm::endpoint = endpoint;
            }

            void setGeometry(Geometry geometry) {
                Osrm::geometry = geometry;
            }

        private:
            std::string endpoint = "localhost:5000";
            Geometry geometry = Geometry::STRAIGHT_LINE;
        };

        class Redis {
        public:
            class Serializer {
            public:
                virtual ~Serializer() = default;
                virtual std::string serializeKey(const Location&, const Location&) = 0;
                virtual std::string serializeRouteDetails(const Location&, const Location&, const RouteDetails&) = 0;
                virtual RouteDetails deserializeRouteDetails(const Location&, const Location&, const std::string&) = 0;
            };

            const std::string &getHost() const {
                return host;
            }

            void setHost(const std::string &host) {
                Redis::host = host;
            }

            size_t getPort() const {
                return port;
            }

            void setPort(size_t port) {
                Redis::port = port;
            }

            bool isForceUpdate() const {
                return force_update;
            }

            void setForceUpdate(bool forceUpdate) {
                force_update = forceUpdate;
            }

            const std::function<std::unique_ptr<Serializer>()> &getSerializerSupplier() const {
                return serializer_supplier;
            }

            void setSerializerSupplier(const std::function<std::unique_ptr<Serializer>()> &serializerSupplier) {
                serializer_supplier = serializerSupplier;
            }

        private:
            std::string host;
            std::size_t port;
            bool force_update = false;
            std::function<std::unique_ptr<Serializer>()> serializer_supplier;
        };

        const Osrm &getOsrmSettings() const {
            return osrmSettings;
        }

        void setOsrmSettings(const Osrm &osrmSettings) {
            RouteProviderSettings::osrmSettings = osrmSettings;
        }

        const Redis &getRedisSettings() const {
            return redisSettings;
        }

        void setRedisSettings(const Redis &redisSettings) {
            RouteProviderSettings::redisSettings = redisSettings;
        }

        bool isEnableRedisCache() const {
            return enableRedisCache;
        }

        void setEnableRedisCache(bool enableRedisCache) {
            RouteProviderSettings::enableRedisCache = enableRedisCache;
        }

    private:
        Osrm osrmSettings;
        Redis redisSettings;
        bool enableRedisCache = false;
    };
}