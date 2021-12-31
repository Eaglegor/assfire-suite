#include "MockRouteProviderEngine.hpp"
#include <spdlog/spdlog.h>
#include "assfire/locations/api/io/Streams.hpp"
#include "assfire/concepts/io/Streams.hpp"
#include <fmt/ostream.h>

using namespace assfire::router;

RouteInfo MockRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    SPDLOG_INFO("Requesting single route info for {}->{}", origin, destination);
    ++calls_count;
    if (origin == destination) return RouteInfo::zero();
    if (indexed_responses.contains(buildKey(origin, destination))) {
        return indexed_responses.at(buildKey(origin, destination)).getSummary();
    }
    if (route_details_responses.empty()) throw std::runtime_error("No mocked responses left");
    RouteInfo result = route_details_responses.front().getSummary();
    route_details_responses.pop();
    SPDLOG_INFO("Result: {}->{} = {}", origin, destination, result.getDistance());
    return result;
}

RouteDetails MockRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    SPDLOG_INFO("Requesting single route info for {}->{}", origin, destination);
    ++calls_count;
    if (origin == destination) return RouteDetails::zero(origin, destination);
    if (indexed_responses.contains(buildKey(origin, destination))) {
        return indexed_responses.at(buildKey(origin, destination));
    }
    if (route_details_responses.empty()) throw std::runtime_error("No mocked responses left");
    RouteDetails result = route_details_responses.front();
    route_details_responses.pop();
    SPDLOG_INFO("Result: {}->{} = {}", origin, destination, result.getDistance());
    return result;
}

void MockRouteProviderEngine::addResponse(double distance, long duration) {
    route_details_responses.push(RouteDetails(RouteInfo(Distance::fromMeters(distance), TimeInterval::fromSeconds(duration)), {}));
    SPDLOG_INFO("Registered mock route response: ({}, {})", distance, duration);
}

void MockRouteProviderEngine::addResponse(double distance, long duration, const RouteDetails::Waypoints &waypoints) {
    route_details_responses.push(RouteDetails(RouteInfo(Distance::fromMeters(distance), TimeInterval::fromSeconds(duration)), waypoints));
    SPDLOG_INFO("Registered mock route response: ({}, {}) + {} waypoints", distance, duration, waypoints.size());
}

void MockRouteProviderEngine::addResponse(const Location &from, const Location &to, const RouteDetails &details) {
    indexed_responses.insert_or_assign(buildKey(from, to), details);
}

std::string MockRouteProviderEngine::buildKey(const Location &from, const Location &to) {
    return std::to_string(from.getLatitude().encodedValue()) + std::to_string(from.getLongitude().encodedValue()) +
           std::to_string(to.getLatitude().encodedValue()) + std::to_string(to.getLongitude().encodedValue());
}

int MockRouteProviderEngine::getCallsCount() const {
    return calls_count;
}
