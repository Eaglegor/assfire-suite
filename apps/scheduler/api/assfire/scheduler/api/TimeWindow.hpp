#pragma once

#include "assfire/concepts/TimePoint.hpp"
#include "assfire/concepts/TimeInterval.hpp"

namespace assfire::scheduler {
    class TimeWindow {
    public:
        TimeWindow(const TimePoint& start_time, const TimePoint& end_time);

        bool operator==(const TimeWindow& tw) const = default;

        TimePoint getStartTime() const;
        TimePoint getEndTime() const;

        TimeInterval getWidth() const;

        static TimeWindow fromEpochSecondsPair(std::int64_t start_epoch_seconds, std::int64_t end_epoch_seconds);

    private:
        TimePoint start_time;
        TimePoint end_time;
    };
}