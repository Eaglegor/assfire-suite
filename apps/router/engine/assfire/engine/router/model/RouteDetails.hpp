#pragma once

#include <utility>
#include <vector>
#include <Location>

namespace assfire::router
{
    class RouteDetails
    {
    public:
        using Waypoints = std::vector<Location>;

        RouteDetails(const RouteInfo &summary,
                     Waypoints waypoints)
                :
                summary(summary),
                waypoints(std::move(waypoints))
        {}

        RouteDetails(const RouteDetails &rhs) = default;

        const RouteInfo &getSummary() const
        {
            return summary;
        }

        const Waypoints &getWaypoints() const
        {
            return waypoints;
        }

    private:
        RouteInfo summary;
        Waypoints waypoints;
    };
}