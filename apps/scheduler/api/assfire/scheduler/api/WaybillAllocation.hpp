#pragma once

#include <vector>
#include <optional>
#include <memory>
#include "assfire/concepts/TimePoint.hpp"
#include "assfire/concepts/TimePoint.hpp"
#include "assfire/concepts/TimeInterval.hpp"
#include "assfire/router/api/RouteInfo.hpp"
#include "assfire/router/api/IndexedLocation.hpp"
#include "TimeWindow.hpp"

namespace assfire::scheduler
{
    class WaybillAllocation
    {
    public:
        using RouteInfo = router::RouteInfo;
        using TimeWindows = std::vector<TimeWindow>;
        using Location = locations::Location;
        using IndexedLocation = router::IndexedLocation;

        WaybillAllocation(const assfire::TimePoint &start_time, const assfire::TimePoint &end_time, const assfire::TimeInterval &planned_duration,
                          WaybillAllocation::TimeWindows time_windows, const Location &location);

        WaybillAllocation(const assfire::TimePoint &start_time, const assfire::TimePoint &end_time, const assfire::TimeInterval &planned_duration,
                          WaybillAllocation::TimeWindows time_windows, const IndexedLocation &location);

        bool operator==(const WaybillAllocation& rhs) const = default;

        TimePoint getStartTime() const;
        TimePoint getEndTime() const;
        TimeInterval getActualDuration() const;
        TimeInterval getPlannedDuration() const;
        const IndexedLocation &getLocation() const;

        const TimeWindows& getTimeWindows() const;
        TimeWindow getNearestNextTimeWindow(TimePoint& tp) const;
        RouteInfo getNextRouteInfo() const;
        void setLocation(const Location& loc);
        void setLocation(const IndexedLocation& loc);

        void setStartTime(const TimePoint& tp);
        void setEndTime(const TimePoint& tp);
        void setNextRouteInfo(const RouteInfo& route_info);

        void shiftStartTime(const TimeInterval& time_interval);

    private:
        TimePoint start_time;
        TimePoint end_time;
        TimeInterval planned_duration;
        TimeWindows time_windows;
        RouteInfo next_route_info;
        IndexedLocation location;
    };
}