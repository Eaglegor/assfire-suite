#include "TimeWindow.hpp"

using namespace assfire::scheduler;

TimeWindow::TimeWindow(const assfire::TimePoint &start_time, const assfire::TimePoint &end_time) :
        start_time(start_time),
        end_time(end_time) {

}

assfire::TimePoint TimeWindow::getStartTime() const {
    return start_time;
}

assfire::TimePoint TimeWindow::getEndTime() const {
    return end_time;
}

assfire::TimeInterval TimeWindow::getWidth() const {
    return end_time - start_time;
}

TimeWindow TimeWindow::fromEpochSecondsPair(std::int64_t start_epoch_seconds, std::int64_t end_epoch_seconds) {
    return TimeWindow(TimePoint::fromEpochSeconds(start_epoch_seconds), TimePoint::fromEpochSeconds(end_epoch_seconds));
}

TimeWindow TimeWindow::infinity() {
    return TimeWindow(TimePoint::zero(), TimePoint::infinity());
}
