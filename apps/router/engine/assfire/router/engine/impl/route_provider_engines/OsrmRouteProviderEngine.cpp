#include "OsrmRouteProviderEngine.hpp"

#include <cpprest/http_client.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "CpprestWstringConvert.hpp"
#include <spdlog/spdlog.h>
#include "assfire/concepts/io/Streams.hpp"
#include "assfire/locations/api/io/Streams.hpp"
#include "CrowflightCalculator.hpp"

using namespace assfire::router;
using namespace web;
using namespace web::http;
using namespace web::http::client;

OsrmRouteProviderEngine::OsrmRouteProviderEngine(const RoutingProfile &routingProfile, const OsrmGeometry &geometry, std::unique_ptr<OsrmConnector> connector) :
        routing_profile(routingProfile),
        geometry(geometry),
        client(std::move(connector)) {}

RouteInfo OsrmRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    metrics_collector.recordSingleOsrmRouteInfoCalculation(geometry);
    return calculateRouteDetails(origin, destination).getSummary();
}

RouteDetails OsrmRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    metrics_collector.recordSingleOsrmRouteDetailsCalculation(geometry);
    return calculateRouteDetails(origin, destination);
}

RouteDetails OsrmRouteProviderEngine::calculateRouteDetails(const Location &origin, const Location &destination) const {
    if (origin == destination) return RouteDetails::zero(origin, destination);
    return getTripDetails({origin, destination})[0];
}

TripInfo OsrmRouteProviderEngine::getTripInfo(const RouteProviderEngine::LocationsList &locations) const {
    return RouteProviderEngine::getTripInfo(locations);
}

TripDetails OsrmRouteProviderEngine::getTripDetails(const RouteProviderEngine::LocationsList &locations) const {
    if(locations.size() < 2) {
        return TripDetails::zero();
    }

    uri_builder builder;
    builder
            .append_path(U("route"))
            .append_path(U("v1"))
            .append_path(U("driving"));

    bool first = true;
    auto locations_path = convert("");
    for (const Location &loc: locations) {
        if (!first) {
            locations_path += U(";");
        }
        first = false;
        locations_path +=convert(std::to_string(loc.getLongitude().doubleValue())) +
                            U(",") +
                            convert(std::to_string(loc.getLatitude().doubleValue()));
    }
    builder.append_path(locations_path);

    bool waypoints_expected = false;
    if (geometry != OsrmGeometry::STRAIGHT_LINE) {
        std::string geometry_format;
        if (geometry == OsrmGeometry::SIMPLIFIED) geometry_format = "simplified";
        else geometry_format = "full";

        builder.append_query(U("overview"), convert(geometry_format), true)
                .append_query(U("geometries"), U("geojson"), true);
        waypoints_expected = true;
    } else {
        builder.append_query(U("overview"), U("false"), true);
    }

    auto request_string = builder.to_string();
    SPDLOG_DEBUG("OSRM request: {}", convert(request_string));

    json::value response = client->requestOsrmRoute(builder.to_string());

    if (response.at(U("code")).as_string() != U("Ok")) {
        auto message = convert(response.at(U("message")).as_string());
        SPDLOG_ERROR("OSRM backend returned error: {}", message);
        throw std::runtime_error(fmt::format("OSRM backend returned error: {}", message));
    }

    std::vector<Location> processed_locations;
    json::value processed_locations_json = response.at(U("waypoints"));
    for (int i = 0; i < locations.size(); ++i) {
        json::value loc = processed_locations_json.at(i).at(U("location"));
        processed_locations.emplace_back(locations::Coordinate::fromDoubleValue(loc.at(1).as_double()), locations::Coordinate::fromDoubleValue(loc.at(0).as_double()));
    }

    json::value best_route = response.at(U("routes")).at(0);

    TimeInterval total_duration = TimeInterval::fromSeconds(best_route.at(U("duration")).as_double());
    Distance total_distance = Distance::fromMeters(best_route.at(U("distance")).as_double());

    RouteDetails::Waypoints all_waypoints;

    if (best_route.has_field(U("geometry"))) {
        for (const auto &g: best_route.at(U("geometry")).at(U("coordinates")).as_array()) {
            all_waypoints.emplace_back(locations::Coordinate::fromDoubleValue(g.at(1).as_double()), locations::Coordinate::fromDoubleValue(g.at(0).as_double()));
        }
    } else {
        if (waypoints_expected) {
            SPDLOG_WARN("Expected list of waypoints but no waypoints were found in OSRM response. Probably something could have been gone wrong");
        }
        std::copy(processed_locations.begin(), processed_locations.end(), std::back_inserter(all_waypoints));
    }
    https://router.project-osrm.org/route/v1/driving/59.0185547,57.2552805;66.4892578,62.1655019;68.9501953,59.0405546?geometries=geojson
    http://router.project-osrm.org/route/v1/driving/13.388860,52.517037;13.397634,52.529407;13.428555,52.523219?overview=false
    RouteDetailsVector legs;

    auto wp_iter = all_waypoints.begin();
    for (int i = 0; i < locations.size() - 1; ++i) {
        json::value route_leg = best_route.at(U("legs")).at(i);
        RouteDetails::Waypoints leg_waypoints;
        TimeInterval leg_duration = TimeInterval::fromSeconds(route_leg.at(U("duration")).as_double());
        Distance leg_distance = Distance::fromMeters(route_leg.at(U("distance")).as_double());
        auto iter = wp_iter;
        if(processed_locations[i] != locations[i]) {
            leg_waypoints.push_back(locations[i]);
        }
        bool cut = false;
        while (iter != all_waypoints.end() && !cut) {
            const RouteDetails::Waypoint wp = *iter;
            leg_waypoints.push_back(wp);
            if(wp == processed_locations[i + 1]) {
                cut = true;
            }
            ++iter;
        }
        if(processed_locations[i + 1] != locations[i + 1]) {
            leg_waypoints.push_back(locations[i + 1]);
        }

        if(processed_locations[i] != locations[i]) {
            Distance cf_distance = Distance::fromMeters(CrowflightCalculator()(
                    leg_waypoints[0].getLatitude().doubleValue(),
                    leg_waypoints[0].getLongitude().doubleValue(),
                    leg_waypoints[1].getLatitude().doubleValue(),
                    leg_waypoints[1].getLongitude().doubleValue()
                    ));
            TimeInterval cf_duration = routing_profile.getSpeed().getTimeToTravel(cf_distance);
            leg_distance += cf_distance;
            leg_duration += cf_duration;
            total_distance += cf_distance;
            total_duration += cf_duration;
        }
        if(processed_locations[i + 1] != locations[i + 1]) {
            Distance cf_distance = Distance::fromMeters(CrowflightCalculator()(
                    leg_waypoints[leg_waypoints.size() - 2].getLatitude().doubleValue(),
                    leg_waypoints[leg_waypoints.size() - 2].getLongitude().doubleValue(),
                    leg_waypoints[leg_waypoints.size() - 1].getLatitude().doubleValue(),
                    leg_waypoints[leg_waypoints.size() - 1].getLongitude().doubleValue()
            ));
            TimeInterval cf_duration = routing_profile.getSpeed().getTimeToTravel(cf_distance);
            leg_distance += cf_distance;
            leg_duration += cf_duration;
            total_distance += cf_distance;
            total_duration += cf_duration;
        }

        wp_iter = iter;
        legs.emplace_back(leg_distance, leg_duration, leg_waypoints);
    }

    SPDLOG_TRACE("OSRM trip calculated [{}:{}]({}) = (dist: {}, time: {})", *locations.begin(), *locations.rbegin(), locations.size(), total_distance, total_duration);

    return TripDetails(total_distance, total_duration, legs);
}
