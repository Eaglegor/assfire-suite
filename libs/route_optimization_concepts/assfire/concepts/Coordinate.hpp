#pragma once

#include <cstdint>

namespace assfire
{
    class Coordinate
    {
    public:
        using value_type = std::int64_t;

    private:
        explicit Coordinate(value_type value);

    public:
        Coordinate();

        Coordinate(const Coordinate &rhs) = default;

        value_type encodedValue() const;

        double doubleValue() const;

        bool operator==(const Coordinate &rhs) const;

        bool operator!=(const Coordinate &rhs) const;

        static Coordinate fromEncodedValue(value_type v);

        static Coordinate fromDoubleValue(double v);

    private:
        value_type data;
    };
}