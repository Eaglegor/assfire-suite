#pragma once

#include <string>

namespace assfire::router {
    enum class OsrmGeometry {
        UNSPECIFIED = 0,
        STRAIGHT_LINE = 1,
        SIMPLIFIED = 2,
        FULL = 3
    };

    class OsrmSettings {
    public:
        enum class Region {
            WORLD
        };

        OsrmGeometry getGeometry() const {
            return geometry;
        }

        void setGeometry(OsrmGeometry geometry) {
            this->geometry = geometry;
        }

        Region getPreferredRegion() const {
            return preferred_region;
        }

        void setPreferredRegion(Region region) {
            OsrmSettings::preferred_region = region;
        }

    private:
        OsrmGeometry geometry = OsrmGeometry::STRAIGHT_LINE;
        Region preferred_region = Region::WORLD;
    };
}