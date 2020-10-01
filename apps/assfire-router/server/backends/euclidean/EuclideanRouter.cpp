#include "EuclideanRouter.hpp"
#include <cmath>

#include <CoordinatesDecoder.hpp>
#include <spdlog/spdlog.h>

using namespace assfire;
using namespace assfire::routing::proto::v1;

namespace {
	bool operator==(const assfire::routing::proto::v1::Location& lhs, const assfire::routing::proto::v1::Location& rhs)	{
		return lhs.lat() == rhs.lat() && lhs.lon() == rhs.lon();
	}
}

EuclideanRouter::EuclideanRouter(RoutingMetricsCollector metrics_context):
	metrics_context(metrics_context)
{
}

GetSingleRouteResponse EuclideanRouter::getRoute(GetSingleRouteRequest request, long request_id) const
{
    auto stopwatch = metrics_context.euclideanRoutesStopwatch(1, RoutingMetricsCollector::RequestMode::SINGLE);
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

    metrics_context.addEuclideanCalculatedRoutes(1, RoutingMetricsCollector::RequestMode::SINGLE);

    GetSingleRouteResponse response;
    response.mutable_route_info()->CopyFrom(result);
    response.mutable_status()->set_code(ResponseStatus::OK);
    return response;
}

routing::proto::v1::GetRoutesBatchResponse EuclideanRouter::getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest request, long request_id) const
{
    auto stopwatch = metrics_context.euclideanRoutesStopwatch(request.origins_size() * request.destinations_size(), RoutingMetricsCollector::RequestMode::BATCH);
    return calculateBatchUsingSingleRoutes(request, request_id);
}

void EuclideanRouter::getRoutesBatch(routing::proto::v1::GetRoutesBatchRequest request, RoutesBatchConsumer routesBatchConsumer, long request_id) const
{
    auto stopwatch = metrics_context.euclideanRoutesStopwatch(request.origins_size() * request.destinations_size(), RoutingMetricsCollector::RequestMode::STREAMING_BATCH);
    routesBatchConsumer(calculateBatchUsingSingleRoutes(request, request_id));
}




