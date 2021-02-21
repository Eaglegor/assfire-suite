#pragma once

#include <cstdint>
#include <compare>

namespace assfire::router {
    class Distance {
    private:
        explicit Distance(double meters) :
                meters(meters) {}

    public:
        auto operator<=>(const Distance& rhs) const = default;

        double toMeters() const {
            return meters;
        }

        static Distance fromMeters(double meters) {
            return Distance(meters);
        }

        static Distance fromKilometers(double kilometers) {
            return Distance(kilometers * 1000);
        }

        static Distance zero() {
            return fromMeters(0);
        }

    private:
        double meters;
    };
}