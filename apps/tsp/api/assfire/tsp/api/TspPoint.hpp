#pragma once

#include <assfire/concepts/Location.hpp>

namespace assfire::tsp
{
    class TspPoint {
    public:
        const Location &getLocation() const {
            return location;
        }

        void setLocation(const Location &location) {
            TspPoint::location = location;
        }

    private:
        Location location;
    };
}