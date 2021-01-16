#pragma once

#include <string>

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

        const Osrm &getOsrmSettings() const {
            return osrmSettings;
        }

        void setOsrmSettings(const Osrm &osrmSettings) {
            RouteProviderSettings::osrmSettings = osrmSettings;
        }

    private:
        Osrm osrmSettings;
    };
}