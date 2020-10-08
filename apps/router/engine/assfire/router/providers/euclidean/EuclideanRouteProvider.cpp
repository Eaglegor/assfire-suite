#include "EuclideanRouteProvider.hpp"
#include <cmath>

#include <assfire/router/CoordinatesDecoder.hpp>
#include <spdlog/spdlog.h>

using namespace assfire::router;

namespace {
	bool operator==(const RouteProvider::Location& lhs, const RouteProvider::Location& rhs)	{
		return lhs.lat() == rhs.lat() && lhs.lon() == rhs.lon();
	}
}

EuclideanRouteProvider::EuclideanRouteProvider(MetricsCollector metrics_collector):
	metrics_collector(metrics_collector)
{
}

RouteProvider::GetSingleRouteResponse EuclideanRouteProvider::getRoute(GetSingleRouteRequest request, long request_id) const
{
    auto stopwatch = metrics_collector.euclideanRoutesStopwatch(1, MetricsCollector::RequestMode::SINGLE);
    RouteInfo result;

    result.mutable_origin()->CopyFrom(request.origin());
    result.mutable_destination()->CopyFrom(request.destination());

    if (request.origin() == request.destination()) {
        result.set_distance(0);
        result.set_duration(0);
    }

    double from_lat = CoordinatesDecoder::decode(request.origin().lat(), request.options().coordinates_format());
    double from_lon = CoordinatesDecoder::decode(request.origin().lon(), request.options().coordinates_format());

    double to_lat = CoordinatesDecoder::decode(request.destination().lat(), request.options().coordinates_format());
    double to_lon = CoordinatesDecoder::decode(request.destination().lon(), request.options().coordinates_format());

    result.set_distance(std::hypot(std::fabs(to_lat - from_lat), std::fabs(to_lon - from_lon)));
    result.set_duration(std::ceil(result.distance() / request.options().velocity()));
    if(request.options().retrieve_waypoints()) {
        result.add_waypoints()->CopyFrom(request.origin());
        result.add_waypoints()->CopyFrom(request.destination());
    }

    SPDLOG_TRACE("[{}]: Euclidean route calculated ({},{})->({},{}) = (dist: {}, time: {})", request_id,
                 request.origin().lat(), request.origin().lon(),
                 request.destination().lat(), request.destination().lon(),
                 result.distance(), result.duration());

    metrics_collector.addEuclideanCalculatedRoutes(1, MetricsCollector::RequestMode::SINGLE);

    GetSingleRouteResponse response;
    response.mutable_route_info()->CopyFrom(result);
    response.mutable_status()->set_code(ResponseStatus::OK);
    return response;
}

RouteProvider::GetRoutesBatchResponse EuclideanRouteProvider::getRoutesBatch(GetRoutesBatchRequest request, long request_id) const
{
    auto stopwatch = metrics_collector.euclideanRoutesStopwatch(request.origins_size() * request.destinations_size(), MetricsCollector::RequestMode::BATCH);
    return calculateBatchUsingSingleRoutes(request, request_id);
}

void EuclideanRouteProvider::getRoutesBatch(GetRoutesBatchRequest request, RoutesBatchConsumer routesBatchConsumer, long request_id) const
{
    auto stopwatch = metrics_collector.euclideanRoutesStopwatch(request.origins_size() * request.destinations_size(), MetricsCollector::RequestMode::STREAMING_BATCH);
    routesBatchConsumer(calculateBatchUsingSingleRoutes(request, request_id));
}




