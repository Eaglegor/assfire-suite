#include "CrowflightRouter.hpp"
#include <cmath>

#include <CoordinatesDecoder.hpp>
#include <spdlog/spdlog.h>

using namespace assfire;
using namespace assfire::routing::proto::v1;

namespace {
	bool operator==(const assfire::routing::proto::v1::Location& lhs, const assfire::routing::proto::v1::Location& rhs)	{
		return lhs.latitude() == rhs.latitude() && lhs.longitude() == rhs.longitude();
	}
}

CrowflightRouter::CrowflightRouter(RoutingMetricsCollector metrics_context):
	metrics_context(metrics_context)
{
}

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

GetSingleRouteResponse CrowflightRouter::getRoute(GetSingleRouteRequest request, long request_id) const
{
    auto stopwatch = metrics_context.crowflightRoutesStopwatch(1, RoutingMetricsCollector::RequestMode::SINGLE);
    RouteInfo result;

    result.mutable_origin()->CopyFrom(request.from());
    result.mutable_destination()->CopyFrom(request.to());

    if (request.from() == request.to()) {
        result.set_distance(0);
        result.set_duration(0);
    }

    double from_lat = CoordinatesDecoder::decode(request.from().latitude(), request.options().coordinates_format());
    double from_lon = CoordinatesDecoder::decode(request.from().longitude(), request.options().coordinates_format());

    double to_lat = CoordinatesDecoder::decode(request.to().latitude(), request.options().coordinates_format());
    double to_lon = CoordinatesDecoder::decode(request.to().longitude(), request.options().coordinates_format());


    result.set_distance(calculateCrowflightDistance(from_lat, from_lon, to_lat, to_lon));
    result.set_duration(std::ceil(result.distance() / request.options().velocity()));

    if(request.options().retrieve_waypoints()) {
        result.add_waypoints()->CopyFrom(request.from());
        result.add_waypoints()->CopyFrom(request.to());
    }

    SPDLOG_TRACE("[{}]: Crowflight route calculated ({},{})->({},{}) = (dist: {}, time: {})", request_id,
                 request.from().latitude(), request.from().longitude(),
                 request.to().latitude(), request.to().longitude(),
                 result.distance(), result.duration());

    metrics_context.addCrowflightCalculatedRoutes(1, RoutingMetricsCollector::RequestMode::SINGLE);

    GetSingleRouteResponse response;
    response.mutable_route_info()->CopyFrom(result);
    return response;
}

routing::proto::v1::GetRoutesBatchResponse CrowflightRouter::getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest request, long request_id) const
{
    auto stopwatch = metrics_context.crowflightRoutesStopwatch(request.origins_size() * request.destinations_size(), RoutingMetricsCollector::RequestMode::BATCH);
    return calculateBatchUsingSingleRoutes(request, request_id);
}

void CrowflightRouter::getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest request, RoutesBatchConsumer routesBatchConsumer, long request_id) const
{
    auto stopwatch = metrics_context.crowflightRoutesStopwatch(request.origins_size() * request.destinations_size(), RoutingMetricsCollector::RequestMode::STREAMING_BATCH);
    routesBatchConsumer(calculateBatchUsingSingleRoutes(request, request_id));
}




