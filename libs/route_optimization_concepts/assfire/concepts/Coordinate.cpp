#include "Coordinate.hpp"

using namespace assfire;

namespace {
    constexpr static double ENCODE_MULTIPLIER = 1e6;
}

Coordinate::Coordinate(Coordinate::value_type value)
        : data(value) {}

Coordinate::Coordinate()
        : data(0) {}

Coordinate::value_type Coordinate::encodedValue() const
{
    return data;
}

double Coordinate::doubleValue() const
{
    return data / ENCODE_MULTIPLIER;
}

bool Coordinate::operator==(const Coordinate &rhs) const
{
    return data == rhs.data;
}

bool Coordinate::operator!=(const Coordinate &rhs) const
{
    return !(rhs == *this);
}

Coordinate Coordinate::fromEncodedValue(Coordinate::value_type v)
{
    return Coordinate(v);
}

Coordinate Coordinate::fromDoubleValue(double v)
{
    return Coordinate(static_cast<value_type>(v * ENCODE_MULTIPLIER));
}
