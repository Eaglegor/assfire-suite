#pragma once

#include <assfire/locations/api/Location.hpp>

namespace assfire::tsp
{
    class TspPoint
    {
    public:
        using Location = locations::Location;

        TspPoint(int id, const Location &location)
                : id(id), location(location) {}

        int getId() const {
            return id;
        }

        const Location &getLocation() const {
            return location;
        }

    private:
        int id;
        Location location;
    };
}