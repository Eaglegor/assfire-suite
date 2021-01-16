#include "CrowflightRouteProviderEngine.hpp"
#include <spdlog/spdlog.h>

using namespace assfire::router;

namespace {
    constexpr double EARTH_RADIUS = 6399000.0;
    constexpr double PI = 3.14159265359;

    double calculateCrowflightDistance(double lat1, double lon1, double lat2, double lon2) {
        return acos(
                cos((lat1 * (PI / 180))) *
                cos((lon1 * (PI / 180))) *
                cos((lat2 * (PI / 180))) *
                cos((lon2 * (PI / 180)))
                +
                cos((lat1 * (PI / 180))) *
                sin((lon1 * (PI / 180))) *
                cos((lat2 * (PI / 180))) *
                sin((lon2 * (PI / 180)))
                +
                sin((lat1 * (PI / 180))) *
                sin((lat2 * (PI / 180)))
        ) * EARTH_RADIUS;
    }
}

CrowflightRouteProviderEngine::CrowflightRouteProviderEngine(const RoutingProfile &routingProfile) : routing_profile(routingProfile) {}

RouteInfo CrowflightRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    if (origin == destination) return RouteInfo(0, 0);

    TimeInterval duration;
    Distance distance;

    distance = calculateCrowflightDistance(origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                                           destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue());
    duration = routing_profile.getSpeed().getTimeToTravel(distance);

    SPDLOG_TRACE("Crowflight route calculated ({},{})->({},{}) = (dist: {}, time: {})",
                 origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                 destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                 distance, duration);

    return RouteInfo(distance, duration);
}

RouteDetails CrowflightRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    return RouteDetails(getSingleRouteInfo(origin, destination), {origin, destination});
}
