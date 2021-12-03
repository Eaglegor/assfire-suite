#pragma once

#include "Coordinate.hpp"

namespace assfire::locations
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
        static Location fromEncodedLatLon(Coordinate::encoded_type lat, Coordinate::encoded_type lon);

    private:
        Coordinate latitude;
        Coordinate longitude;
    };
}