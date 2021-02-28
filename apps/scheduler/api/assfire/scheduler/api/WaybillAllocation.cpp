#include "WaybillAllocation.hpp"

#include <utility>

using namespace assfire::scheduler;

WaybillAllocation::WaybillAllocation(const assfire::TimePoint &start_time, const assfire::TimePoint &end_time, const assfire::TimeInterval &planned_duration,
                                     WaybillAllocation::TimeWindows time_windows, const WaybillAllocation::RouteInfo &next_route) :
        start_time(start_time),
        end_time(end_time),
        planned_duration(planned_duration),
        time_windows(std::move(time_windows)),
        next_route(next_route) {
}

assfire::TimePoint WaybillAllocation::getStartTime() const {
    return start_time;
}

assfire::TimePoint WaybillAllocation::getEndTime() const {
    return end_time;
}

assfire::TimeInterval WaybillAllocation::getActualDuration() const {
    return end_time - start_time;
}

assfire::TimeInterval WaybillAllocation::getPlannedDuration() const {
    return planned_duration;
}

TimeWindow WaybillAllocation::getNearestNextTimeWindow(assfire::TimePoint &tp) const {
    return *time_windows.begin();
}

WaybillAllocation::RouteInfo WaybillAllocation::getNextRouteInfo() const {
    return next_route;
}

void WaybillAllocation::setStartTime(const assfire::TimePoint &tp) {
    start_time = tp;
}

void WaybillAllocation::setEndTime(const assfire::TimePoint &tp) {
    end_time = tp;
}

void WaybillAllocation::shiftStartTime(const assfire::TimeInterval &time_interval) {
    start_time += time_interval;
    end_time += time_interval;
}

const WaybillAllocation::TimeWindows &WaybillAllocation::getTimeWindows() const {
    return time_windows;
}
