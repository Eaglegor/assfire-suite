#include "EuclideanRouteProviderEngine.hpp"

#include <spdlog/spdlog.h>
#include <cmath>

using namespace assfire::router;

namespace {
    double calculateEuclideanDistance(double lat1, double lon1, double lat2, double lon2) {
        return std::hypot(std::fabs(lat2 - lat1), std::fabs(lon2 - lon1));
    }
}

EuclideanRouteProviderEngine::EuclideanRouteProviderEngine(const RoutingProfile &routingProfile) : routing_profile(routingProfile) {}

RouteInfo EuclideanRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    if (origin == destination) return RouteInfo(0, 0);

    TimeInterval duration;
    Distance distance;

    distance = calculateEuclideanDistance(origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                                           destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue());
    duration = routing_profile.getSpeed().getTimeToTravel(distance);

    SPDLOG_TRACE("Euclidean route calculated ({},{})->({},{}) = (dist: {}, time: {})",
                 origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                 destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                 distance, duration);

    return RouteInfo(distance, duration);
}

RouteDetails EuclideanRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    return RouteDetails(getSingleRouteInfo(origin, destination), {origin, destination});
}
