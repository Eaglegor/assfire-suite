#pragma once

#include "Distance.hpp"
#include "Interval.hpp"

namespace assfire::router {
    class Speed {
    public:
        Speed(Distance distancePerTimeUnit) : distance_per_time_unit(distancePerTimeUnit) {}

        Distance getDistancePerTime(const TimeInterval &interval) const {
            return distance_per_time_unit;
        }

        Distance getDistancePerTimeUnit() const {
            return getDistancePerTime(1);
        }

        TimeInterval getTimeToTravel(const Distance &distance) {
            return distance / distance_per_time_unit;
        }

    private:
        Distance distance_per_time_unit;
    };
}