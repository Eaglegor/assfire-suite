#pragma once

#include "Distance.hpp"
#include "TimeInterval.hpp"

namespace assfire::router {
    class RouteInfo {
    public:
        RouteInfo()
                :
                distance(Distance::zero()),
                duration(TimeInterval::zero()) {}

        RouteInfo(const Distance &distance, const TimeInterval &duration)
                :
                distance(distance),
                duration(duration) {}

        RouteInfo(const RouteInfo &rhs) = default;

        const Distance &getDistance() const {
            return distance;
        }

        const TimeInterval &getDuration() const {
            return duration;
        }

        static RouteInfo zero() {
            return RouteInfo(Distance::zero(), TimeInterval::zero());
        }

    private:
        Distance distance;
        TimeInterval duration;
    };
}