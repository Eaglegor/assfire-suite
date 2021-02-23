#include "MockRouteProviderEngine.hpp"
#include <spdlog/spdlog.h>

using namespace assfire::router;

RouteInfo MockRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    ++calls_count;
    if (origin == destination) return RouteInfo::zero();
    if(route_details_responses.empty()) throw std::runtime_error("No mocked responses left");
    RouteInfo result = route_details_responses.front().getSummary();
    route_details_responses.pop();
    return result;
}

RouteDetails MockRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    ++calls_count;
    if (origin == destination) return RouteDetails();
    if(route_details_responses.empty()) throw std::runtime_error("No mocked responses left");
    RouteDetails result = route_details_responses.front();
    route_details_responses.pop();
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

int MockRouteProviderEngine::getCallsCount() const {
    return calls_count;
}
