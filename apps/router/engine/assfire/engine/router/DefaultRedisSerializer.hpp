#pragma once

#include <sstream>
#include <assfire/api/router/RouteProviderSettings.hpp>
#include <assfire/api/router/RedisSerializer.hpp>

namespace assfire::router {
    // [TODO] Write proper portable binary serialization
    class DefaultRedisSerializer : public RedisSerializer {
    public:
        DefaultRedisSerializer(RouterEngineType engine_type, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) {
            std::ostringstream str;

            str << static_cast<int>(engine_type) << " " << routing_profile.getSpeed().getDistancePerTimeUnit() << " " << settings.isRetrieveWaypoints() << " " << static_cast<int>(settings.getOsrmSettings().getGeometry());

            suffix = str.str();
        }

        std::string serializeKey(const Location &origin, const Location &destination) override {
            std::ostringstream str;
            str << origin.getLatitude().longValue()
                << origin.getLongitude().longValue()
                << destination.getLatitude().longValue()
                << destination.getLongitude().longValue()
                << suffix;
            return str.str();
        }

        std::string serializeRouteDetails(const Location &origin, const Location &destination, const RouteDetails &details) override {
            std::ostringstream str;
            str << details.getSummary().getDuration() << " "
                << details.getSummary().getDistance() << " ";

            str << details.getWaypoints().size() << " ";
            for (const RouteDetails::Waypoint &waypoint : details.getWaypoints()) {
                str << waypoint.getLatitude().longValue()
                    << waypoint.getLongitude().longValue();
            }

            return str.str();
        }

        RouteDetails deserializeRouteDetails(const Location &origin, const Location &destination, const std::string &serialized_route_details) override {
            std::istringstream str(serialized_route_details);
            RouteDetails result;
            TimeInterval duration;
            Distance distance;
            str >> duration >> distance;

            RouteInfo summary(distance, duration);

            int waypoints_size;
            str >> waypoints_size;

            RouteDetails::Waypoints waypoints;
            if (waypoints_size > 0) {
                waypoints.reserve(waypoints_size);
                for (int i = 0; i < waypoints_size; ++i) {
                    long lat, lon;
                    str >> lat >> lon;
                    waypoints.push_back(RouteDetails::Waypoint(Coordinate(lat), Coordinate(lon)));
                }
            }

            return RouteDetails(summary, waypoints);
        }

    private:
        std::string suffix;
    };
}