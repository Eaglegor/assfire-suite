#pragma once

#include "TimePoint.hpp"
#include <cstdint>
#include <compare>
#include <limits>

namespace assfire {
    class TimeInterval;

    class TimePoint {
    private:
        using value_type = std::int64_t;

        explicit TimePoint(value_type epoch_seconds) : epoch_seconds(epoch_seconds) {}

    public:
        TimePoint() : epoch_seconds(0) {}

        auto operator<=>(const TimePoint& rhs) const = default;

        std::int64_t toEpochSeconds() const;

        static TimePoint fromEpochSeconds(std::int64_t epoch_seconds);

        TimePoint& operator+=(const TimeInterval& rhs);
        TimePoint& operator-=(const TimeInterval& rhs);
        TimePoint operator+(const TimeInterval& rhs) const;
        TimePoint operator-(const TimeInterval& rhs) const;
        TimeInterval operator-(const TimePoint& rhs) const;

        static TimePoint zero() {
            return TimePoint::fromEpochSeconds(0);
        }

        static TimePoint infinity() {
            return TimePoint::fromEpochSeconds(std::numeric_limits<value_type>::max());
        }

    private:
        value_type epoch_seconds;
    };

}

namespace std
{
    assfire::TimePoint min(const assfire::TimePoint& lhs, const assfire::TimePoint& rhs);
    assfire::TimePoint max(const assfire::TimePoint& lhs, const assfire::TimePoint& rhs);
}