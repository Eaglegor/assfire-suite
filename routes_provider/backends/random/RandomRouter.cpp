#include "RandomRouter.hpp"
#include <cmath>

#include <spdlog/spdlog.h>

using namespace assfire;
using namespace assfire::routing::proto;

namespace {
	bool operator==(const assfire::routing::proto::Location& lhs, const assfire::routing::proto::Location& rhs)	{
		return lhs.latitude() == rhs.latitude() && lhs.longitude() == rhs.longitude();
	}
}

RandomRouter::RandomRouter(RoutingMetricsCollector metrics_context):
	metrics_context(metrics_context),
	gen(rd()),
	distr(1000.0, 100000.0)
{
}

RouteInfo RandomRouter::getRoute(SingleRouteRequest request, long request_id) const
{
	RouteInfo result;

	result.mutable_origin()->CopyFrom(request.from());
	result.mutable_destination()->CopyFrom(request.to());

	if (request.from() == request.to()) {
		result.set_distance(0);
		result.set_duration(0);
	}

	result.set_distance(distr(gen));
	result.set_duration(result.distance() / 16.6);

	SPDLOG_TRACE("[{}]: Random route calculated ({},{})->({},{}) = (dist: {}, time: {})", request_id,
		request.from().latitude(), request.from().longitude(),
		request.to().latitude(), request.to().longitude(),
		result.distance(), result.duration());

	metrics_context.addRandomCalculatedRoutes(1, RoutingMetricsCollector::RequestMode::SINGLE);
	return result;
}

void RandomRouter::getRoutesBatch(ManyToManyRoutesRequest request, std::function<void(RouteInfo)> callback, long request_id) const
{
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
