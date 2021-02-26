#pragma once

#include "Distance.hpp"
#include "TimeInterval.hpp"
#include <cmath>

namespace assfire
{
    class Speed
    {
    private:
        explicit Speed(double meters_per_second);

    public:
        double toMetersPerSecond() const;

        double toMetersPerSecondWithPrecision(double precision) const;

        double toKilometersPerHour() const;

        TimeInterval getSecondsToTravel(const Distance &distance);

        static Speed fromMetersPerSecond(double value);

        static Speed fromKilometersPerHour(double value);

    private:
        double meters_per_second;
    };
}