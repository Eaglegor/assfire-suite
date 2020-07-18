#include "EuclideanRouter.hpp"
#include <cmath>

#include <CoordinatesDecoder.hpp>
#include <spdlog/spdlog.h>

using namespace assfire;
using namespace assfire::routing::proto;

namespace {
	bool operator==(const assfire::routing::proto::Location& lhs, const assfire::routing::proto::Location& rhs)	{
		return lhs.latitude() == rhs.latitude() && lhs.longitude() == rhs.longitude();
	}
}

EuclideanRouter::EuclideanRouter(RoutingMetricsCollector metrics_context):
	metrics_context(metrics_context)
{
}

RouteInfo EuclideanRouter::getRoute(SingleRouteRequest request, long request_id) const
{
	auto stopwatch = metrics_context.euclideanRoutesStopwatch(1, RoutingMetricsCollector::RequestMode::SINGLE);
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

	result.set_distance(std::hypot(std::fabs(to_lat - from_lat), std::fabs(to_lon - from_lon)));
	result.set_duration(result.distance() / request.options().euclidean_routing_options().velocity());

	SPDLOG_TRACE("[{}]: Euclidean route calculated ({},{})->({},{}) = (dist: {}, time: {})", request_id,
		request.from().latitude(), request.from().longitude(),
		request.to().latitude(), request.to().longitude(),
		result.distance(), result.duration());

	metrics_context.addEuclideanCalculatedRoutes(1, RoutingMetricsCollector::RequestMode::SINGLE);
	return result;
}

void EuclideanRouter::getRoutesBatch(ManyToManyRoutesRequest request, std::function<void(RouteInfo)> callback, long request_id) const
{
	auto stopwatch = metrics_context.euclideanRoutesStopwatch(request.origins_size() * request.destinations_size(), RoutingMetricsCollector::RequestMode::BATCH);
	for (const Location& origin : request.origins()) {
		for (const Location& destination : request.destinations()) {
			SingleRouteRequest single_request;
			single_request.mutable_from()->CopyFrom(origin);
			single_request.mutable_to()->CopyFrom(destination);
			single_request.mutable_options()->CopyFrom(request.options());
			callback(getRoute(single_request, request_id));
		}
	}
}
