#include "EuclideanRouteProviderEngine.hpp"

#include <spdlog/spdlog.h>
#include <cmath>
#include <utility>
#include <fmt/ostream.h>
#include "assfire/locations/api/io/Streams.hpp"
#include "assfire/concepts/io/Streams.hpp"

using namespace assfire::router;

namespace
{
    double calculateEuclideanDistance(double lat1, double lon1, double lat2, double lon2) {
        return std::hypot(std::fabs(lat2 - lat1), std::fabs(lon2 - lon1));
    }
}

EuclideanRouteProviderEngine::EuclideanRouteProviderEngine(const RoutingProfile &routing_profile, EngineMetricsCollector metrics_collector)
        : routing_profile(routing_profile),
          metrics_collector(std::move(metrics_collector)) {}

RouteInfo EuclideanRouteProviderEngine::getSingleRouteInfo(const Location &origin, const Location &destination) const {
    metrics_collector.recordSingleEuclideanRouteInfoCalculation();
    return calculateRouteInfo(origin, destination);
}

RouteDetails EuclideanRouteProviderEngine::getSingleRouteDetails(const Location &origin, const Location &destination) const {
    metrics_collector.recordSingleEuclideanRouteDetailsCalculation();
    return RouteDetails(calculateRouteInfo(origin, destination), {origin, destination});
}

RouteInfo EuclideanRouteProviderEngine::calculateRouteInfo(const Location &origin, const Location &destination) const {
    if (origin == destination) return RouteInfo::zero();

    auto stopwatch = metrics_collector.measureSingleEuclideanRouteInfoCalculation();

    Distance distance = Distance::fromMeters(calculateEuclideanDistance(origin.getLatitude().doubleValue(), origin.getLongitude().doubleValue(),
                                                                        destination.getLatitude().doubleValue(), destination.getLongitude().doubleValue()));
    TimeInterval duration = routing_profile.getSpeed().getTimeToTravel(distance);

    SPDLOG_TRACE("Euclidean route calculated {}->{} = (dist: {}, time: {})", origin, destination, distance, duration);

    return RouteInfo(distance, duration);
}
