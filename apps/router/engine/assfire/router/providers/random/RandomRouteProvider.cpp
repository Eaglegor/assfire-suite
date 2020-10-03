#include "RandomRouteProvider.hpp"
#include <cmath>

#include <spdlog/spdlog.h>

using namespace assfire::router;

namespace {
	bool operator==(const RouteProvider::Location& lhs, const RouteProvider::Location& rhs)	{
		return lhs.lat() == rhs.lat() && lhs.lon() == rhs.lon();
	}
}

RandomRouteProvider::RandomRouteProvider(MetricsCollector metrics_collector):
	metrics_collector(metrics_collector),
	gen(rd()),
	distr(1000.0, 100000.0)
{
}

RouteProvider::GetSingleRouteResponse RandomRouteProvider::getRoute(GetSingleRouteRequest request, long request_id) const
{
    auto stopwatch = metrics_collector.randomRoutesStopwatch(1, MetricsCollector::RequestMode::SINGLE);

    RouteInfo result;

    result.mutable_origin()->CopyFrom(request.origin());
    result.mutable_destination()->CopyFrom(request.destination());

    if (request.origin() == request.destination()) {
        result.set_distance(0);
        result.set_duration(0);
    }

    result.set_distance(distr(gen));
    result.set_duration(std::ceil(result.distance() / request.options().velocity()));
    if(request.options().retrieve_waypoints()) {
        result.add_waypoints()->CopyFrom(request.origin());
        result.add_waypoints()->CopyFrom(request.destination());
    }

    SPDLOG_TRACE("[{}]: Random route calculated ({},{})->({},{}) = (dist: {}, time: {})", request_id,
                 request.origin().lat(), request.origin().lon(),
                 request.destination().lat(), request.destination().lon(),
                 result.distance(), result.duration());

    metrics_collector.addRandomCalculatedRoutes(1, MetricsCollector::RequestMode::SINGLE);

    GetSingleRouteResponse response;
    response.mutable_route_info()->CopyFrom(result);
    response.mutable_status()->set_code(ResponseStatus::OK);
    return response;
}

RouteProvider::GetRoutesBatchResponse RandomRouteProvider::getRoutesBatch(GetRoutesBatchRequest request, long request_id) const
{
    auto stopwatch = metrics_collector.randomRoutesStopwatch(request.origins_size() * request.destinations_size(), MetricsCollector::RequestMode::BATCH);
    return calculateBatchUsingSingleRoutes(request, request_id);
}

void RandomRouteProvider::getRoutesBatch(GetRoutesBatchRequest request, RoutesBatchConsumer consumer, long request_id) const
{
    auto stopwatch = metrics_collector.randomRoutesStopwatch(request.origins_size() * request.destinations_size(), MetricsCollector::RequestMode::STREAMING_BATCH);
    consumer(calculateBatchUsingSingleRoutes(request, request_id));
}

