#include "Speed.hpp"

using namespace assfire;

assfire::Speed::Speed(double meters_per_second)
        : meters_per_second(meters_per_second)
{}

double assfire::Speed::toMetersPerSecond() const
{
    return meters_per_second;
}

double assfire::Speed::toMetersPerSecondWithPrecision(double precision) const
{
    return round(meters_per_second / precision) * precision;
}

double assfire::Speed::toKilometersPerHour() const
{
    return meters_per_second / 1000.0 * 3600;
}

assfire::TimeInterval assfire::Speed::getSecondsToTravel(const assfire::Distance &distance)
{
    return TimeInterval::fromSeconds(distance.toMeters() / meters_per_second);
}

assfire::Speed assfire::Speed::fromMetersPerSecond(double value)
{
    return Speed(value);
}

assfire::Speed assfire::Speed::fromKilometersPerHour(double value)
{
    return Speed(value / 3600.0 * 1000);
}
