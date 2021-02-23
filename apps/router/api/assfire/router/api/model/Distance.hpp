#pragma once

#include <cstdint>
#include <compare>
#include <cmath>
#include <limits>

namespace assfire::router {
    class Distance {
    private:
        explicit Distance(double meters) :
                meters(meters) {}

        static constexpr double INFINITE_DISTANCE = 1000000000.0;

    public:
        auto operator<=>(const Distance &rhs) const = default;

        double toMeters() const {
            return meters;
        }

        static Distance fromMeters(double meters) {
            return Distance(meters);
        }

        static Distance fromKilometers(double kilometers) {
            return Distance(kilometers * 1000);
        }

        bool isInfinity() const {
            return meters >= INFINITE_DISTANCE;
        }

        bool isZero() const {
            return std::fabs(meters) < std::numeric_limits<double>::epsilon();
        }

        static Distance zero() {
            return fromMeters(0);
        }

        static Distance infinity() {
            return fromMeters(INFINITE_DISTANCE);
        }

    private:
        double meters;
    };
}