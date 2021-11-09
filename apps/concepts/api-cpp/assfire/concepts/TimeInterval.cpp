#include "TimeInterval.hpp"

using namespace assfire;

std::int64_t TimeInterval::toSeconds() const {
    return seconds;
}

TimeInterval TimeInterval::fromSeconds(std::int64_t seconds) {
    return TimeInterval(seconds);
}

bool TimeInterval::isInfinity() const {
    return seconds >= INFINITE_TIME_INTERVAL;
}

bool TimeInterval::isZero() const {
    return seconds == 0;
}

TimeInterval TimeInterval::zero() {
    return fromSeconds(0);
}

TimeInterval TimeInterval::infinity() {
    return fromSeconds(INFINITE_TIME_INTERVAL);
}

TimeInterval TimeInterval::operator+(const TimeInterval &rhs) const {
    return TimeInterval::fromSeconds(seconds + rhs.seconds);
}

TimeInterval TimeInterval::operator-(const TimeInterval &rhs) const {
    return TimeInterval::fromSeconds(seconds - rhs.seconds);
}

TimeInterval &TimeInterval::operator+=(const TimeInterval &rhs) {
    seconds += rhs.seconds;
    return *this;
}

TimeInterval &TimeInterval::operator-=(const TimeInterval &rhs) {
    seconds -= rhs.seconds;
    return *this;
}

bool TimeInterval::isPositive() const {
    return seconds > 0;
}

TimePoint &TimePoint::operator+=(const TimeInterval &rhs) {
    epoch_seconds += rhs.toSeconds();
    return *this;
}

TimePoint &TimePoint::operator-=(const TimeInterval &rhs) {
    epoch_seconds -= rhs.toSeconds();
    return *this;
}

TimePoint TimePoint::operator+(const TimeInterval &rhs) const {
    return TimePoint::fromEpochSeconds(epoch_seconds + rhs.toSeconds());
}

TimePoint TimePoint::operator-(const TimeInterval &rhs) const {
    return TimePoint::fromEpochSeconds(epoch_seconds - rhs.toSeconds());
}

TimeInterval TimePoint::operator-(const TimePoint &rhs) const {
    return TimeInterval::fromSeconds(epoch_seconds - rhs.toEpochSeconds());
}


