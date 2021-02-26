#include "Distance.hpp"

using namespace assfire;

namespace {
    static constexpr double INFINITE_DISTANCE = 1000000000.0;
}

Distance::Distance(double meters)
        :
        meters(meters) {}

double Distance::toMeters() const
{
    return meters;
}

Distance Distance::fromMeters(double meters)
{
    return Distance(meters);
}

Distance Distance::fromKilometers(double kilometers)
{
    return Distance(kilometers * 1000);
}

bool Distance::isInfinity() const
{
    return meters >= INFINITE_DISTANCE;
}

bool Distance::isZero() const
{
    return std::fabs(meters) < std::numeric_limits<double>::epsilon();
}

Distance Distance::zero()
{
    return fromMeters(0);
}

Distance Distance::infinity()
{
    return fromMeters(INFINITE_DISTANCE);
}
