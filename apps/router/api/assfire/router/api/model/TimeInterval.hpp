#pragma once

#include "TimePoint.hpp"
#include <compare>

namespace assfire::router {
    class TimeInterval {
    private:
        using value_type = decltype(TimePoint() - TimePoint());

        explicit TimeInterval(value_type seconds) : seconds(seconds) {}

        static constexpr value_type INFINITE_TIME_INTERVAL = 3600000000;

    public:
        auto operator<=>(const TimeInterval& rhs) const = default;

        std::int64_t toSeconds() const {
            return seconds;
        }

        static TimeInterval fromSeconds(std::int64_t seconds) {
            return TimeInterval(seconds);
        }

        bool isInfinity() const {
            return seconds >= INFINITE_TIME_INTERVAL;
        }

        bool isZero() const {
            return seconds == 0;
        }

        static TimeInterval zero() {
            return fromSeconds(0);
        }

        static TimeInterval infinity() {
            return fromSeconds(INFINITE_TIME_INTERVAL);
        }

    private:
        value_type seconds;
    };

}