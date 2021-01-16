#pragma once

#include "Distance.hpp"
#include "Interval.hpp"

namespace assfire::router
{
    class RouteInfo
    {
    public:
        RouteInfo(const Distance &distance, const Interval &duration)
                :
                distance(distance),
                duration(duration)
        {}

        RouteInfo(const RouteInfo &rhs) = default;

        const Distance &getDistance() const
        {
            return distance;
        }

        const Interval &getDuration() const
        {
            return duration;
        }

    private:
        Distance distance;
        Interval duration;
    };
}