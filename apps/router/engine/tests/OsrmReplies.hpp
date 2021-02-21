#pragma once

#include <string>
#include <assfire/api/router/model/RouteDetails.hpp>
#include <assfire/engine/router/impl/route_provider_engines/CpprestWstringConvert.hpp>
#include <cpprest/json.h>

namespace assfire::router::test::osrm_replies {

    std::wstring buildRequest(const Location &origin, const Location &destination, const std::string &overview) {
        std::string result;
        result += "/route/v1/driving/";
        result += std::to_string(origin.getLongitude().doubleValue()) + ",";
        result += std::to_string(origin.getLatitude().doubleValue()) + ";";
        result += std::to_string(destination.getLongitude().doubleValue()) + ",";
        result += std::to_string(destination.getLatitude().doubleValue());
        result += "?";
        result += "overview=" + overview;
        return convert(result);
    }

    std::wstring createHint(const Location& loc) {
        return L"AYa5kXOGuZEYAAAAJgAAAAAAAAAAAAAASjFaQU1xpUEAAAAAAAAAAAwAAAATAAAAAAAAAAAAAABXGAEA_kvMAKlYIQM8TMwArVghAwAA7wodsCRh";
    }


    web::json::value buildResponse(const Location &origin, const Location &destination, const RouteDetails &route_details) {
        using value = web::json::value;

        value result;

        result[L"code"] = value::string(L"Ok");

        value& waypoints = result[L"waypoints"] = value::array(2);

        value& json_origin = waypoints[0];
        json_origin[L"hint"] = value::string(createHint(origin));
        json_origin[L"distance"] = 0;
        value& json_origin_location = json_origin[L"location"] = value::array(2);
        json_origin_location[0] = origin.getLongitude().doubleValue();
        json_origin_location[1] = origin.getLatitude().doubleValue();
        json_origin[L"name"] = value::string(L"Origin");

        value& json_destination = waypoints[1];
        json_destination[L"hint"] = value::string(createHint(destination));
        json_destination[L"distance"] = 0;
        value& json_destination_location = json_destination[L"location"] = value::array(2);
        json_destination_location[0] = destination.getLongitude().doubleValue();
        json_destination_location[1] = destination.getLatitude().doubleValue();
        json_destination[L"name"] = value::string(L"Destination");

        value& routes = result[L"routes"] = value::array(1);
        value& route = routes[0];

        value& legs = route[L"legs"] = value::array(1);
        value& leg = legs[0];

        leg[L"steps"] = value::array();
        leg[L"weight"] = (int32_t) route_details.getDuration().toSeconds();
        leg[L"distance"] = route_details.getDistance().toMeters();
        leg[L"duration"] = (int32_t) route_details.getDuration().toSeconds();
        leg[L"summary"] = value();

        route[L"weight_name"] = value::string(L"routability");
        route[L"weight"] = (int32_t) route_details.getDuration().toSeconds();
        route[L"distance"] = route_details.getDistance().toMeters();
        route[L"duration"] = (int32_t) route_details.getDuration().toSeconds();

        if (!route_details.getWaypoints().empty()) {
            value &geometry = route[L"geometry"];
            geometry[L"type"] = value::string(L"LineString");
            value &coordinates = geometry[L"coordinates"] = value::array(route_details.getWaypoints().size());
            for (int i = 0; i < route_details.getWaypoints().size(); ++i) {
                value &json_waypoint = coordinates[i] = value::array(2);
                json_waypoint[0] = route_details.getWaypoints()[i].getLongitude().doubleValue();
                json_waypoint[1] = route_details.getWaypoints()[i].getLatitude().doubleValue();
            }
        }

        return result;
    }
}