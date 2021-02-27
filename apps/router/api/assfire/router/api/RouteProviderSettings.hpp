#pragma once

#include <string>
#include <functional>

namespace assfire::router {
    enum class OsrmGeometry {
        UNSPECIFIED = 0,
        STRAIGHT_LINE = 1,
        SIMPLIFIED = 2,
        FULL = 3
    };

    class OsrmSettings {
    public:
        OsrmGeometry getGeometry() const {
            return geometry;
        }

        void setGeometry(OsrmGeometry geometry) {
            this->geometry = geometry;
        }

    private:
        OsrmGeometry geometry = OsrmGeometry::STRAIGHT_LINE;
    };

    class RouteProviderSettings {
    public:


        const OsrmSettings &getOsrmSettings() const {
            return osrm_settings;
        }

        void setOsrmSettings(const OsrmSettings &osrm_settings) {
            RouteProviderSettings::osrm_settings = osrm_settings;
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
        OsrmSettings osrm_settings;
        bool force_update = false;
        bool retrieve_waypoints = false;
    };
}