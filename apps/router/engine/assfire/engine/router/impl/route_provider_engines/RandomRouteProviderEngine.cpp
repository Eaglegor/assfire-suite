#include "RandomRouteProviderEngine.hpp"

#include <cmath>
#include <spdlog/spdlog.h>

using namespace assfire::router;

RandomRouteProviderEngine::RandomRouteProviderEngine(const RoutingProfile &routingProfile) : routing_profile(routingProfile),
                                                                                             metrics_collector(metrics_collector),
                                                                                             gen(rd()),
                                                                                             distr(1000.0, 100000.0) {}

RouteInfo RandomRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    if (origin == destination) return RouteInfo(0, 0);

    TimeInterval duration;
    Distance distance;

    distance = distr(gen);
    duration = routing_profile.getSpeed().getTimeToTravel(distance);

    SPDLOG_TRACE("Random route calculated ({},{})->({},{}) = (dist: {}, time: {})",
                 origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                 destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                 distance, duration);

    return RouteInfo(distance, duration);
}

RouteDetails RandomRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    return RouteDetails(getSingleRouteInfo(origin, destination), {origin, destination});
}
