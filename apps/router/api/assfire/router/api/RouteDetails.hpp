#pragma once

#include <utility>
#include <vector>
#include "assfire/locations/api/Location.hpp"
#include "RouteInfo.hpp"

namespace assfire::router {
    class RouteDetails {
    public:
        using Location = locations::Location;
        using Waypoint = Location;
        using Waypoints = std::vector<Waypoint>;

        RouteDetails() :
                summary(RouteInfo::zero()) {}

        RouteDetails(const RouteInfo &summary,
                     Waypoints waypoints)
                :
                summary(summary),
                waypoints(std::move(waypoints)) {}

        RouteDetails(const Distance& distance,
                     const TimeInterval& time_interval,
                     Waypoints waypoints)
                :
                summary(distance, time_interval),
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

        bool isInfinity() const {
            return summary.isInfinity();
        }

        bool isZero() const {
            return summary.isZero();
        }

        static const RouteDetails zero(const Location& from, const Location& to) {
            return RouteDetails(RouteInfo::zero(), {from, to});
        }

        static const RouteDetails infinity(const Location& from, const Location& to) {
            return RouteDetails(RouteInfo::infinity(), {from, to});
        }

    private:
        RouteInfo summary;
        Waypoints waypoints;
    };
}