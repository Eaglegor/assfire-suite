#pragma once

namespace assfire::osrm {
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
}