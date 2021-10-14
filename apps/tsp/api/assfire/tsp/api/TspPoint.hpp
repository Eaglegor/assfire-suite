#pragma once

#include <assfire/concepts/Location.hpp>

namespace assfire::tsp
{
    class TspPoint {
    public:
        TspPoint(int id, const Location &location) : id(id), location(location) {}

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