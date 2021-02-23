#pragma once

#include "Distance.hpp"
#include "TimeInterval.hpp"
#include <cmath>

namespace assfire::router {
    class Speed {
    private:
        explicit Speed(double meters_per_second) : meters_per_second(meters_per_second) {}
    public:
        double toMetersPerSecond() const {
            return meters_per_second;
        }

        double toMetersPerSecondWithPrecision(double precision) const {
            return round(meters_per_second / precision) * precision;
        }

        double toKilometersPerHour() const {
            return meters_per_second / 1000.0 * 3600;
        }

        TimeInterval getSecondsToTravel(const Distance &distance) {
            return TimeInterval::fromSeconds(distance.toMeters() / meters_per_second);
        }

        static Speed fromMetersPerSecond(double value) {
            return Speed(value);
        }

        static Speed fromKilometersPerHour(double value) {
            return Speed(value / 3600.0 * 1000);
        }

    private:
        double meters_per_second;
    };
}