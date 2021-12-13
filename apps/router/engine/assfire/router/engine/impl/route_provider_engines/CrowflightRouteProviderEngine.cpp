#include "CrowflightRouteProviderEngine.hpp"
#include <spdlog/spdlog.h>
#include <fmt/ostream.h>
#include "assfire/locations/api/io/Streams.hpp"
#include "assfire/concepts/io/Streams.hpp"
#include "CrowflightCalculator.hpp"

using namespace assfire::router;

CrowflightRouteProviderEngine::CrowflightRouteProviderEngine(const RoutingProfile &routingProfile, EngineMetricsCollector metrics_collector)
        : routing_profile(routingProfile),
          metrics_collector(std::move(metrics_collector)) {}

RouteInfo CrowflightRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    metrics_collector.recordSingleCrowflightRouteInfoCalculation();
    return calculateRouteInfo(origin, destination);
}

RouteDetails CrowflightRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    metrics_collector.recordSingleCrowflightRouteDetailsCalculation();
    return RouteDetails(calculateRouteInfo(origin, destination), {origin, destination});
}

RouteInfo CrowflightRouteProviderEngine::calculateRouteInfo(const Location &origin, const Location &destination) const {
    if (origin == destination) return RouteInfo::zero();

    auto stopwatch = metrics_collector.measureSingleCrowflightRouteInfoCalculation();

    Distance distance = Distance::fromMeters(CrowflightCalculator()(origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                                                                         destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue()));
    TimeInterval duration = routing_profile.getSpeed().getTimeToTravel(distance);

    SPDLOG_TRACE("Crowflight route calculated {}->{} = (dist: {}, time: {})", origin, destination, distance, duration);

    return RouteInfo(distance, duration);
}
