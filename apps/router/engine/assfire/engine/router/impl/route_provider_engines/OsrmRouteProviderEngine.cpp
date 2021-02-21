#include "OsrmRouteProviderEngine.hpp"

#include <cmath>
#include <cpprest/http_client.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <codecvt>
#include <locale>

using namespace assfire::router;
using namespace web;
using namespace web::http;
using namespace web::http::client;

namespace {
#ifdef WIN32

    std::string convert(const utility::string_t &from) {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(from);
    }

    utility::string_t convert(const std::string &from) {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(from);
    }

#else
    std::string convert(const std::string& from)
    {
        return from;
    }
#endif
}

OsrmRouteProviderEngine::OsrmRouteProviderEngine(const std::string &osrm_endpoint, const RoutingProfile &routingProfile, const RouteProviderSettings::Osrm::Geometry &geometry) :
        routing_profile(routingProfile),
        geometry(geometry) {}

RouteInfo OsrmRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    return getSingleRouteDetails(origin, destination).getSummary();
}

RouteDetails OsrmRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    if (origin == destination) return RouteDetails(RouteInfo::zero(), {origin, destination});

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
    if (geometry != RouteProviderSettings::Osrm::Geometry::STRAIGHT_LINE) {
        std::string geometry_format;
        if (geometry == RouteProviderSettings::Osrm::Geometry::SIMPLIFIED) geometry_format = "simplified";
        else geometry_format = "full";

        builder.append_query(U("overview"), convert(geometry_format), true)
                .append_query(U("geometries"), U("geojson"), true);
        waypoints_expected = true;
    } else {
        builder.append_query(U("overview"), U("false"), true);
    }

    auto request_string = builder.to_string();
    SPDLOG_DEBUG("OSRM request: {}", convert(request_string));

    auto task = client->request(methods::GET, builder.to_string())
            .then([](const http_response &response) {
                      SPDLOG_DEBUG("OSRM response: {}", convert(response.to_string()));

                      return response.extract_json();
                  }
            ).then([&origin, &destination, waypoints_expected](const json::value &response) {
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

                SPDLOG_TRACE("OSRM route calculated ({},{})->({},{}) = (dist: {}, time: {})",
                             origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                             destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                             distance.toMeters(), duration.toSeconds());

                return RouteDetails(RouteInfo(distance, duration), waypoints);
            });

    return task.get();
}
