#pragma once

#include "Coordinate.hpp"

namespace assfire::router
{
    class Location
    {
    public:
        Location() = default;

        Location(const Coordinate &latitude, const Coordinate &longitude)
                :
                latitude(latitude),
                longitude(longitude)
        {}

        Location(const Location& rhs) = default;

        const Coordinate& getLatitude() const
        {
            return latitude;
        }

        const Coordinate& getLongitude() const
        {
            return longitude;
        }

    private:
        Coordinate latitude;
        Coordinate longitude;
    };
}