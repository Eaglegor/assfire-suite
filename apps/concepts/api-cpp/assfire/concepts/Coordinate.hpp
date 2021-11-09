#pragma once

#include <cstdint>

namespace assfire
{
    class Coordinate
    {
    public:
        using encoded_type = std::int64_t;

    private:
        explicit Coordinate(encoded_type value);

    public:
        Coordinate();

        Coordinate(const Coordinate &rhs) = default;

        encoded_type encodedValue() const;

        double doubleValue() const;

        bool operator==(const Coordinate &rhs) const;

        bool operator!=(const Coordinate &rhs) const;

        static Coordinate fromEncodedValue(encoded_type v);

        static Coordinate fromDoubleValue(double v);

    private:
        encoded_type data;
    };
}