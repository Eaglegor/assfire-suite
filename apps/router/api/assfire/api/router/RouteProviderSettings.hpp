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

            Geometry getGeometry() const {
                return geometry;
            }

            void setGeometry(Geometry geometry) {
                Osrm::geometry = geometry;
            }

        private:
            Geometry geometry = Geometry::STRAIGHT_LINE;
        };

        const Osrm &getOsrmSettings() const {
            return osrmSettings;
        }

        void setOsrmSettings(const Osrm &osrmSettings) {
            RouteProviderSettings::osrmSettings = osrmSettings;
        }

        bool isForceUpdate() const {
            return force_update;
        }

        void setForceUpdate(bool forceUpdate) {
            force_update = forceUpdate;
        }

        bool isRetrieveWaypoints() const {
            return retrieve_waypoints;
        }

        void setRetrieveWaypoints(bool retrieveWaypoints) {
            retrieve_waypoints = retrieveWaypoints;
        }

    private:
        Osrm osrmSettings;
        bool force_update = false;
        bool retrieve_waypoints = false;
    };
}