#pragma once

#include "Distance.hpp"
#include "Interval.hpp"

namespace assfire::router
{
    class RouteInfo
    {
    public:
        RouteInfo():
        distance(0),
        duration(0)
        {}

        RouteInfo(const Distance &distance, const TimeInterval &duration)
                :
                distance(distance),
                duration(duration)
        {}

        RouteInfo(const RouteInfo &rhs) = default;

        const Distance &getDistance() const
        {
            return distance;
        }

        const TimeInterval &getDuration() const
        {
            return duration;
        }

    private:
        Distance distance;
        TimeInterval duration;
    };
}