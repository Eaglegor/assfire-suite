#include "TimePoint.hpp"

using namespace assfire;

std::int64_t TimePoint::toEpochSeconds() const {
    return epoch_seconds;
}

TimePoint TimePoint::fromEpochSeconds(std::int64_t epoch_seconds) {
    return TimePoint(epoch_seconds);
}
