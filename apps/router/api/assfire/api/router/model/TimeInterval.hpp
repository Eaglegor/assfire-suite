#pragma once

#include "TimePoint.hpp"

namespace assfire::router {
    class TimeInterval {
    private:
        using value_type = decltype(TimePoint() - TimePoint());

        explicit TimeInterval(value_type seconds) : seconds(seconds) {}

    public:
        long toSeconds() const {
            return seconds;
        }

        static TimeInterval fromSeconds(long seconds) {
            return TimeInterval(seconds);
        }

        static TimeInterval zero() {
            return fromSeconds(0);
        }

    private:
        value_type seconds;
    };

}