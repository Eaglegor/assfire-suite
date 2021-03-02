#include "WaybillAllocation.hpp"

#include <utility>

using namespace assfire::router;
using namespace assfire::scheduler;

WaybillAllocation::WaybillAllocation(const assfire::TimePoint &start_time, const assfire::TimePoint &end_time, const assfire::TimeInterval &planned_duration,
                                     WaybillAllocation::TimeWindows time_windows, const assfire::Location &location) :
        start_time(start_time),
        end_time(end_time),
        planned_duration(planned_duration),
        time_windows(std::move(time_windows)),
        location(-1, IndexedLocation::INVALID_TAG, location){
}

WaybillAllocation::WaybillAllocation(const assfire::TimePoint &start_time, const assfire::TimePoint &end_time, const assfire::TimeInterval &planned_duration,
                                     WaybillAllocation::TimeWindows time_windows, const IndexedLocation &location) :
        start_time(start_time),
        end_time(end_time),
        planned_duration(planned_duration),
        time_windows(std::move(time_windows)),
        location(location) {
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
    return next_route_info;
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

const WaybillAllocation::Location &WaybillAllocation::getLocation() const {
    return location;
}

void WaybillAllocation::setNextRouteInfo(const WaybillAllocation::RouteInfo &route_info) {
    this->next_route_info = route_info;
}

void WaybillAllocation::setLocation(const assfire::Location &loc) {
    location = IndexedLocation(-1, IndexedLocation::INVALID_TAG, loc);
}

void WaybillAllocation::setLocation(const IndexedLocation &loc) {
    location = loc;
}

