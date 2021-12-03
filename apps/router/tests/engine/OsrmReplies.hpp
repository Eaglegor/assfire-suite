#pragma once

#include <string>
#include <assfire/router/api/RouteDetails.hpp>
#include <assfire/router/engine/impl/route_provider_engines/CpprestWstringConvert.hpp>
#include <cpprest/json.h>

namespace assfire::router::test::osrm_replies {

    utility::string_t buildRequest(const locations::Location &origin, const locations::Location &destination, const std::string &overview) {
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

    utility::string_t createHint(const locations::Location& loc) {
        return convert("AYa5kXOGuZEYAAAAJgAAAAAAAAAAAAAASjFaQU1xpUEAAAAAAAAAAAwAAAATAAAAAAAAAAAAAABXGAEA_kvMAKlYIQM8TMwArVghAwAA7wodsCRh");
    }


    web::json::value buildResponse(const locations::Location &origin, const locations::Location &destination, const RouteDetails &route_details) {
        using value = web::json::value;

        value result;

        result[convert("code")] = value::string(convert("Ok"));

        value& waypoints = result[convert("waypoints")] = value::array(2);

        value& json_origin = waypoints[0];
        json_origin[convert("hint")] = value::string(createHint(origin));
        json_origin[convert("distance")] = 0;
        value& json_origin_location = json_origin[convert("location")] = value::array(2);
        json_origin_location[0] = origin.getLongitude().doubleValue();
        json_origin_location[1] = origin.getLatitude().doubleValue();
        json_origin[convert("name")] = value::string(convert("Origin"));

        value& json_destination = waypoints[1];
        json_destination[convert("hint")] = value::string(createHint(destination));
        json_destination[convert("distance")] = 0;
        value& json_destination_location = json_destination[convert("location")] = value::array(2);
        json_destination_location[0] = destination.getLongitude().doubleValue();
        json_destination_location[1] = destination.getLatitude().doubleValue();
        json_destination[convert("name")] = value::string(convert("Destination"));

        value& routes = result[convert("routes")] = value::array(1);
        value& route = routes[0];

        value& legs = route[convert("legs")] = value::array(1);
        value& leg = legs[0];

        leg[convert("steps")] = value::array();
        leg[convert("weight")] = (int32_t) route_details.getDuration().toSeconds();
        leg[convert("distance")] = route_details.getDistance().toMeters();
        leg[convert("duration")] = (int32_t) route_details.getDuration().toSeconds();
        leg[convert("summary")] = value();

        route[convert("weight_name")] = value::string(convert("routability"));
        route[convert("weight")] = (int32_t) route_details.getDuration().toSeconds();
        route[convert("distance")] = route_details.getDistance().toMeters();
        route[convert("duration")] = (int32_t) route_details.getDuration().toSeconds();

        if (!route_details.getWaypoints().empty()) {
            value &geometry = route[convert("geometry")];
            geometry[convert("type")] = value::string(convert("LineString"));
            value &coordinates = geometry[convert("coordinates")] = value::array(route_details.getWaypoints().size());
            for (int i = 0; i < route_details.getWaypoints().size(); ++i) {
                value &json_waypoint = coordinates[i] = value::array(2);
                json_waypoint[0] = route_details.getWaypoints()[i].getLongitude().doubleValue();
                json_waypoint[1] = route_details.getWaypoints()[i].getLatitude().doubleValue();
            }
        }

        return result;
    }
}