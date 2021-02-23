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

        bool isInfinity() const {
            return distance.isInfinity() || duration.isInfinity();
        }

        bool isZero() const {
            return distance.isZero() && duration.isZero();
        }

        static RouteInfo zero() {
            return RouteInfo(Distance::zero(), TimeInterval::zero());
        }

        static RouteInfo infinity() {
            return RouteInfo(Distance::infinity(), TimeInterval::infinity());
        }

    private:
        Distance distance;
        TimeInterval duration;
    };
}