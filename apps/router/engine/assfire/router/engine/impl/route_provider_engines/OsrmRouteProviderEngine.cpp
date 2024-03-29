#include "OsrmRouteProviderEngine.hpp"

#include <cpprest/http_client.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "CpprestWstringConvert.hpp"
#include <spdlog/spdlog.h>
#include "assfire/concepts/io/Streams.hpp"

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

RouteDetails OsrmRouteProviderEngine::calculateRouteDetails(const assfire::Location &origin, const assfire::Location &destination) const {
    if (origin == destination) return RouteDetails::zero(origin, destination);

    auto stopwatch = metrics_collector.measureSingleOsrmRouteDetailsCalculation(geometry);

    uri_builder builder;
    builder
            .append_path(U("route"))
            .append_path(U("v1"))
            .append_path(U("driving"));

    builder.append_path(
            convert(std::to_string(origin.getLongitude().doubleValue())) +
            U(",") +
            convert(std::to_string(origin.getLatitude().doubleValue())) +
            U(";") +
            convert(std::to_string(destination.getLongitude().doubleValue())) +
            U(",") +
            convert(std::to_string(destination.getLatitude().doubleValue())));

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

    json::value response = client->requestRouteInfo(builder.to_string());

    if (response.at(U("code")).as_string() != U("Ok")) {
        auto message = convert(response.at(U("message")).as_string());
        SPDLOG_ERROR("OSRM backend returned error: {}", message);
        throw std::runtime_error(fmt::format("OSRM backend returned error: {}", message));
    }

    TimeInterval duration = TimeInterval::fromSeconds(response.at(U("routes")).at(0).at(U("duration")).as_double());
    Distance distance = Distance::fromMeters(response.at(U("routes")).at(0).at(U("distance")).as_double());

    RouteDetails::Waypoints waypoints;

    if (response.at(U("routes")).at(0).has_field(U("geometry"))) {
        for (const auto &g : response.at(U("routes")).at(0).at(U("geometry")).at(U("coordinates")).as_array()) {
            waypoints.emplace_back(Coordinate::fromDoubleValue(g.at(1).as_double()), Coordinate::fromDoubleValue(g.at(0).as_double()));
        }
    } else {
        if (waypoints_expected) {
            SPDLOG_WARN("Expected list of waypoints but no waypoins were found in OSRM response. Probably something could have been gone wrong");
        }
        waypoints.emplace_back(origin);
        waypoints.emplace_back(destination);
    }

    SPDLOG_TRACE("OSRM route calculated {}->{} = (dist: {}, time: {})", origin, destination, distance, duration);


    return RouteDetails(distance, duration, waypoints);
}
