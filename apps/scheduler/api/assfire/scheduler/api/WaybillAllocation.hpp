#pragma once

#include <vector>
#include "assfire/concepts/TimePoint.hpp"
#include "assfire/concepts/TimePoint.hpp"
#include "assfire/concepts/TimeInterval.hpp"
#include "assfire/router/api/RouteInfo.hpp"
#include "assfire/concepts/Location.hpp"
#include "TimeWindow.hpp"

namespace assfire::scheduler
{
    class WaybillAllocation
    {
    public:
        using RouteInfo = router::RouteInfo;
        using TimeWindows = std::vector<TimeWindow>;

        WaybillAllocation(const assfire::TimePoint &start_time, const assfire::TimePoint &end_time, const assfire::TimeInterval &planned_duration,
                          WaybillAllocation::TimeWindows time_windows, const WaybillAllocation::RouteInfo &next_route, const Location &location);

        TimePoint getStartTime() const;
        TimePoint getEndTime() const;
        TimeInterval getActualDuration() const;
        TimeInterval getPlannedDuration() const;
        const Location &getLocation() const;

        const TimeWindows& getTimeWindows() const;
        TimeWindow getNearestNextTimeWindow(TimePoint& tp) const;
        RouteInfo getNextRouteInfo() const;

        void setStartTime(const TimePoint& tp);
        void setEndTime(const TimePoint& tp);

        void shiftStartTime(const TimeInterval& time_interval);

    private:
        TimePoint start_time;
        TimePoint end_time;
        TimeInterval planned_duration;
        TimeWindows time_windows;
        RouteInfo next_route;
        Location location;
    };
}