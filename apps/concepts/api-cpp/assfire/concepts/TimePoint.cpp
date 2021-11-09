#include "TimePoint.hpp"

using namespace assfire;

std::int64_t TimePoint::toEpochSeconds() const {
    return epoch_seconds;
}

TimePoint TimePoint::fromEpochSeconds(std::int64_t epoch_seconds) {
    return TimePoint(epoch_seconds);
}

assfire::TimePoint std::min(const TimePoint &lhs, const TimePoint &rhs) {
    if(lhs.toEpochSeconds() < rhs.toEpochSeconds()) return lhs;
    else return rhs;
}

assfire::TimePoint std::max(const TimePoint &lhs, const TimePoint &rhs) {
    if(lhs.toEpochSeconds() > rhs.toEpochSeconds()) return lhs;
    else return rhs;
}
