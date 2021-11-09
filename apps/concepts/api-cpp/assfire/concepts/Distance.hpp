#pragma once

#include <cstdint>
#include <compare>
#include <cmath>
#include <limits>

namespace assfire {
    class Distance {
    private:
        explicit Distance(double meters);

    public:
        auto operator<=>(const Distance &rhs) const = default;

        double toMeters() const;

        static Distance fromMeters(double meters);

        static Distance fromKilometers(double kilometers);

        bool isInfinity() const;

        bool isZero() const;

        static Distance zero();

        static Distance infinity();

    private:
        double meters;
    };
}