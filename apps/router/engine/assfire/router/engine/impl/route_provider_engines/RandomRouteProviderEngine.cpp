#include "RandomRouteProviderEngine.hpp"

#include <cmath>
#include <spdlog/spdlog.h>

using namespace assfire::router;

RandomRouteProviderEngine::RandomRouteProviderEngine(const RoutingProfile &routingProfile, EngineMetricsCollector metrics_collector)
        : routing_profile(routingProfile),
          gen(rd()),
          distr(1000.0, 100000.0),
          metrics_collector(std::move(metrics_collector))
{}

RouteInfo RandomRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const
{
    metrics_collector.recordSingleRandomRouteInfoCalculation();
    return calculateRouteInfo(origin, destination);
}

RouteDetails RandomRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const
{

    metrics_collector.recordSingleRandomRouteDetailsCalculation();
    return RouteDetails(calculateRouteInfo(origin, destination), {origin, destination});
}

RouteInfo RandomRouteProviderEngine::calculateRouteInfo(const Location &origin, const Location &destination) const
{
    if (origin == destination) return RouteInfo::zero();

    auto stopwatch = metrics_collector.measureSingleRandomRouteInfoCalculation();

    Distance distance = Distance::fromMeters(distr(gen));
    TimeInterval duration = routing_profile.getSpeed().getTimeToTravel(distance);

    SPDLOG_TRACE("Random route calculated ({},{})->({},{}) = (dist: {}, time: {})",
                 origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                 destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue(),
                 distance.toMeters(), duration.toSeconds());

    return RouteInfo(distance, duration);
}
