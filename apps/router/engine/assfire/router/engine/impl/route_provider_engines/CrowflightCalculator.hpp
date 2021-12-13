#pragma once

#include <cmath>

namespace assfire::router {
    struct CrowflightCalculator
    {
        static constexpr double EARTH_RADIUS = 6399000.0;
        static constexpr double PI = 3.14159265359;

        double operator()(double lat1, double lon1, double lat2, double lon2) {
            return acos(
                    cos((lat1 * (PI / 180))) *
                    cos((lon1 * (PI / 180))) *
                    cos((lat2 * (PI / 180))) *
                    cos((lon2 * (PI / 180)))
                    +
                    cos((lat1 * (PI / 180))) *
                    sin((lon1 * (PI / 180))) *
                    cos((lat2 * (PI / 180))) *
                    sin((lon2 * (PI / 180)))
                    +
                    sin((lat1 * (PI / 180))) *
                    sin((lat2 * (PI / 180)))
            ) * EARTH_RADIUS;
        }
    };
}