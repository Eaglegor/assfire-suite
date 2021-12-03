#pragma once

#include <sstream>
#include <assfire/router/api/RouteProviderSettings.hpp>
#include <assfire/router/api/RedisSerializer.hpp>
#include <assfire/router/api/RouterEngineType.hpp>
#include <cmath>

namespace assfire::router {
    // [TODO] Write proper portable binary serialization
    class DefaultRedisSerializer : public RedisSerializer {
    public:
        DefaultRedisSerializer(RouterEngineType engine_type, const RoutingProfile &routing_profile, const RouteProviderSettings &settings) {
            std::ostringstream str;

            str << static_cast<int>(engine_type) << " " <<  routing_profile.getSpeed().toMetersPerSecondWithPrecision(0.001) << " " << settings.isRetrieveWaypoints() << " " << static_cast<int>(settings.getOsrmSettings().getGeometry());

            suffix = str.str();
        }

        std::string serializeKey(const Location &origin, const Location &destination) override {
            std::ostringstream str;
            str << origin.getLatitude().encodedValue() << " "
                << origin.getLongitude().encodedValue() << " "
                << destination.getLatitude().encodedValue() << " "
                << destination.getLongitude().encodedValue() << " "
                << suffix;
            return str.str();
        }

        std::string serializeRouteDetails(const Location &origin, const Location &destination, const RouteDetails &details) override {
            std::ostringstream str;
            str << details.getSummary().getDuration().toSeconds() << " "
                << details.getSummary().getDistance().toMeters() << " ";

            str << details.getWaypoints().size() << " ";
            for (const RouteDetails::Waypoint &waypoint : details.getWaypoints()) {
                str << waypoint.getLatitude().encodedValue() << " "
                    << waypoint.getLongitude().encodedValue() << " ";
            }

            return str.str();
        }

        RouteDetails deserializeRouteDetails(const Location &origin, const Location &destination, const std::string &serialized_route_details) override {
            std::istringstream str(serialized_route_details);
            RouteDetails result;
            long seconds;
            double meters;
            str >> seconds >> meters;

            RouteInfo summary(Distance::fromMeters(meters), TimeInterval::fromSeconds(seconds));

            int waypoints_size;
            str >> waypoints_size;

            RouteDetails::Waypoints waypoints;
            if (waypoints_size > 0) {
                waypoints.reserve(waypoints_size);
                for (int i = 0; i < waypoints_size; ++i) {
                    long lat, lon;
                    str >> lat >> lon;
                    waypoints.emplace_back(locations::Coordinate::fromEncodedValue(lat), locations::Coordinate::fromEncodedValue(lon));
                }
            }

            return RouteDetails(summary, waypoints);
        }

    private:
        std::string suffix;
    };
}