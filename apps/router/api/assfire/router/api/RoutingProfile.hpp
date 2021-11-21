#pragma once

#include "assfire/concepts/Speed.hpp"

namespace assfire::router {
    class RoutingProfile {
    public:
        RoutingProfile(const Speed &speed) : speed(speed) {}

        Speed getSpeed() const {
            return speed;
        }

    private:
        Speed speed;
    };
}