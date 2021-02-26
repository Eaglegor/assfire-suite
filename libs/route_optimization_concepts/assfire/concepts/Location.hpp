#pragma once

#include "Coordinate.hpp"

namespace assfire
{
    class Location
    {
    public:
        Location() = default;

        Location(const Coordinate &latitude, const Coordinate &longitude);

        Location(const Location& rhs) = default;

        const Coordinate& getLatitude() const;

        const Coordinate& getLongitude() const;

        bool operator==(const Location &rhs) const;

        bool operator!=(const Location &rhs) const;

        static Location fromDoubleLatLon(double lat, double lon);

    private:
        Coordinate latitude;
        Coordinate longitude;
    };
}