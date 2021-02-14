#pragma once

namespace assfire::router {
    class Coordinate {
    public:
        static constexpr double PRECISION = 1000000.0;

        Coordinate()
                : data(0) {}

        explicit Coordinate(long value)
                : data(value) {}

        explicit Coordinate(double value)
                : data(static_cast<long>(value * PRECISION)) {}

        double doubleValue() const {
            return data / PRECISION;
        }

        long longValue() const {
            return data;
        }

        bool operator==(const Coordinate &rhs) const {
            return data == rhs.data;
        }

        bool operator!=(const Coordinate &rhs) const {
            return !(rhs == *this);
        }

    private:
        long data;
    };
}