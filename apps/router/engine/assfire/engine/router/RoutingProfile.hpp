#pragma once

#include "model/Speed.hpp"

namespace assfire::router {
    class RoutingProfile {
    public:
        Speed getSpeed() const {
            return speed;
        }

    private:
        Speed speed;
    };
}