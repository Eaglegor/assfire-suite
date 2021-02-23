#pragma once
#include <cstdint>

namespace assfire::router {
    class Coordinate {
    public:
        using value_type = std::int64_t;
    private:
        constexpr static double ENCODE_MULTIPLIER = 1e6;

        explicit Coordinate(value_type value)
                : data(value) {}

    public:
        Coordinate()
                : data(0) {}

        Coordinate(const Coordinate &rhs) = default;

        value_type encodedValue() const {
            return data;
        }

        double doubleValue() const {
            return data / ENCODE_MULTIPLIER;
        }

        bool operator==(const Coordinate &rhs) const {
            return data == rhs.data;
        }

        bool operator!=(const Coordinate &rhs) const {
            return !(rhs == *this);
        }

        static Coordinate fromEncodedValue(value_type v) {
            return Coordinate(v);
        }

        static Coordinate fromDoubleValue(double v) {
            return Coordinate(static_cast<value_type>(v * ENCODE_MULTIPLIER));
        }

    private:
        value_type data;
    };
}