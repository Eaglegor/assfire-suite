#pragma once

#include <utility>
#include <vector>
#include "Location.hpp"
#include "RouteInfo.hpp"

namespace assfire::router {
    class RouteDetails {
    public:
        using Waypoint = Location;
        using Waypoints = std::vector<Waypoint>;

        RouteDetails() :
                summary(RouteInfo::zero()) {}

        RouteDetails(const RouteInfo &summary,
                     Waypoints waypoints)
                :
                summary(summary),
                waypoints(std::move(waypoints)) {}

        RouteDetails(const RouteDetails &rhs) = default;

        const RouteInfo &getSummary() const {
            return summary;
        }

        const Distance& getDistance() const {
            return summary.getDistance();
        }

        const TimeInterval& getDuration() const {
            return summary.getDuration();
        }

        const Waypoints &getWaypoints() const {
            return waypoints;
        }

    private:
        RouteInfo summary;
        Waypoints waypoints;
    };
}