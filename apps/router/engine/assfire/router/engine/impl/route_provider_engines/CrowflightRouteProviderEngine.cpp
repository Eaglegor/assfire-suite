#include "CrowflightRouteProviderEngine.hpp"
#include <spdlog/spdlog.h>
#include <fmt/ostream.h>
#include "assfire/locations/api/io/Streams.hpp"
#include "assfire/concepts/io/Streams.hpp"

using namespace assfire::router;

namespace
{
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

    Distance distance = Distance::fromMeters(calculateCrowflightDistance(origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                                                                         destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue()));
    TimeInterval duration = routing_profile.getSpeed().getSecondsToTravel(distance);

    SPDLOG_TRACE("Crowflight route calculated {}->{} = (dist: {}, time: {})", origin, destination, distance, duration);

    return RouteInfo(distance, duration);
}
