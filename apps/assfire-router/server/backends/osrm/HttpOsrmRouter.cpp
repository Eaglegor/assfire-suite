#include "HttpOsrmRouter.hpp"
#include <cpprest/http_client.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <codecvt>
#include <locale>
#include <CoordinatesDecoder.hpp>
#include <CoordinatesEncoder.hpp>

using namespace assfire;
using namespace assfire::routing::proto::v1;
using namespace web;
using namespace web::http;
using namespace web::http::client;

namespace {
#ifdef WIN32

    std::string convert(const utility::string_t &from)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(from);
    }

    utility::string_t convert(const std::string &from)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(from);
    }

#else
    std::string convert(const std::string& from)
    {
        return from;
    }
#endif
}

HttpOsrmRouter::HttpOsrmRouter(const std::string &osrm_endpoint, const RoutingMetricsCollector &metrics_context) :
        metrics_context(metrics_context)
{
    SPDLOG_INFO("Creating OSRM HTTP client for {}", osrm_endpoint);
    client = std::make_unique<http_client>(convert(osrm_endpoint));
}

GetSingleRouteResponse HttpOsrmRouter::getRoute(GetSingleRouteRequest request, long request_id) const
{
    uri_builder builder;
    builder
            .append_path(U("route"))
            .append_path(U("v1"))
            .append_path(U("driving"));

    double from_lat = CoordinatesDecoder::decode(request.origin().lat(), request.options().coordinates_format());
    double from_lon = CoordinatesDecoder::decode(request.origin().lon(), request.options().coordinates_format());

    double to_lat = CoordinatesDecoder::decode(request.destination().lat(), request.options().coordinates_format());
    double to_lon = CoordinatesDecoder::decode(request.destination().lon(), request.options().coordinates_format());

    builder.append_path(
            std::to_wstring(from_lat) +
            U(",") +
            std::to_wstring(from_lon) +
            U(";") +
            std::to_wstring(to_lat) +
            U(",") +
            std::to_wstring(to_lon));

    bool waypoints_expected = false;
    if (request.options().retrieve_waypoints()) {
        if (!request.options().has_osrm()) {
            SPDLOG_WARN("[{}] Waypoints are requested but osrm options are not present in the query. Not requesting waypoints from OSRM because desired format is not known. Please specify osrm.geometry option value", request_id);
            builder.append_query(U("overview"), U("false"), true);
        } else if (request.options().osrm().geometry() == RoutingOptions::Osrm::STRAIGHT_LINE) {
            SPDLOG_INFO("[{}] Waypoints in STRAIGHT_LINE format are requested. Not requesting geometry from OSRM as we can calculate it ourselves", request_id);
            builder.append_query(U("overview"), U("false"), true);
        } else {
            SPDLOG_INFO("[{}] Geometry will be requested from OSRM", request_id);
            std::string geometry_format = (!request.options().has_osrm() || request.options().osrm().geometry() == RoutingOptions::Osrm::SIMPLIFIED) ? "simplified" : "full";
            builder.append_query(U("overview"), convert(geometry_format), true)
                    .append_query(U("geometries"), U("geojson"), true);
            waypoints_expected = true;
        }
    } else {
        builder.append_query(U("overview"), U("false"), true);
    }

    auto request_string = builder.to_string();
    SPDLOG_DEBUG("[{}] OSRM request: {}", request_id, convert(request_string));

    auto task = client->request(methods::GET, builder.to_string())
            .then([request_id](const http_response &response) {
                      SPDLOG_DEBUG("[{}] OSRM response: {}", request_id, convert(response.to_string()));

                      return response.extract_json();
                  }
            ).then([&request, request_id, waypoints_expected](const json::value &response) {
                GetSingleRouteResponse result;
                if (response.at(U("code")).as_string() != U("Ok")) {
                    auto message = convert(response.at(U("message")).as_string());
                    SPDLOG_ERROR("[{}] OSRM backend returned error: {}", request_id, message);
                    result.mutable_status()->set_code(ResponseStatus::ERROR);
                    result.mutable_status()->set_message(fmt::format("OSRM backend returned error: {}", message));
                    return result;
                }

                double duration = response.at(U("routes")).at(0).at(U("duration")).as_double();
                double distance = response.at(U("routes")).at(0).at(U("distance")).as_double();

                result.mutable_route_info()->set_duration(static_cast<decltype(result.mutable_route_info()->duration())>(duration));
                result.mutable_route_info()->set_distance(distance);

                if (response.at(U("routes")).at(0).has_field(U("geometry"))) {
                    for (const auto &g : response.at(U("routes")).at(0).at(U("geometry")).at(U("coordinates")).as_array()) {
                        Location *loc = result.mutable_route_info()->add_waypoints();
                        loc->set_lat(CoordinatesEncoder::encode(g.at(0).as_double(), request.options().coordinates_format()));
                        loc->set_lon(CoordinatesEncoder::encode(g.at(1).as_double(), request.options().coordinates_format()));
                    }
                } else if (waypoints_expected) {
                    SPDLOG_WARN("[{}] Expected list of waypoints but no waypoins were found in OSRM response. Probably something could have been gone wrong", request_id);
                } else if (request.options().retrieve_waypoints() && request.options().has_osrm() && request.options().osrm().geometry() == RoutingOptions::Osrm::STRAIGHT_LINE) {
                    result.mutable_route_info()->add_waypoints()->CopyFrom(request.origin());
                    result.mutable_route_info()->add_waypoints()->CopyFrom(request.destination());
                }

                return result;
            });

    try {
        auto result = task.get();
        result.mutable_route_info()->mutable_origin()->CopyFrom(request.origin());
        result.mutable_route_info()->mutable_destination()->CopyFrom(request.destination());
        result.mutable_status()->set_code(ResponseStatus::OK);
        return result;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("[{}] Error occurred during route calculation: {}", request_id, e.what());
        GetSingleRouteResponse result;
        result.mutable_status()->set_code(ResponseStatus::ERROR);
        result.mutable_status()->set_message(fmt::format("Error occurred during route calculation: {}", e.what()));
        return result;
    }
}

GetRoutesBatchResponse HttpOsrmRouter::getRoutesBatch(GetRoutesBatchRequest request, long request_id) const
{
    return calculateRoutesBatch(request, request_id);
}

void HttpOsrmRouter::getRoutesBatch(GetRoutesBatchRequest request, Router::RoutesBatchConsumer routesBatchConsumer, long request_id) const
{
    routesBatchConsumer(calculateRoutesBatch(request, request_id));
}

GetRoutesBatchResponse HttpOsrmRouter::calculateRoutesBatch(GetRoutesBatchRequest request, long request_id) const
{
    return calculateBatchUsingSingleRoutes(std::move(request), request_id);
}

HttpOsrmRouter::~HttpOsrmRouter() = default;
