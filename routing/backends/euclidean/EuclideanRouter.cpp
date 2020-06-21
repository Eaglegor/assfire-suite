#include "EuclideanRouter.hpp"
#include <cmath>

#include <CoordinatesDecoder.hpp>

using namespace assfire;
using namespace assfire::routing;

RouteInfo EuclideanRouter::getRoute(SingleRouteRequest request) const
{
	RouteInfo result;

	result.mutable_origin()->CopyFrom(request.from());
	result.mutable_destination()->CopyFrom(request.to());

	double from_lat = CoordinatesDecoder::decode(request.from().latitude(), request.options().coordinates_format());
	double from_lon = CoordinatesDecoder::decode(request.from().longitude(), request.options().coordinates_format());

	double to_lat = CoordinatesDecoder::decode(request.to().latitude(), request.options().coordinates_format());
	double to_lon = CoordinatesDecoder::decode(request.to().longitude(), request.options().coordinates_format());

	result.set_distance(std::hypot(std::fabs(to_lat - from_lat), std::fabs(to_lon - from_lon)));

	return result;
}

void EuclideanRouter::getRoutesBatch(ManyToManyRoutesRequest request, std::function<void(RouteInfo)> callback) const
{
	for (const Location& origin : request.origins()) {
		for (const Location& destination : request.destinations()) {
			SingleRouteRequest single_request;
			single_request.mutable_from()->CopyFrom(origin);
			single_request.mutable_to()->CopyFrom(destination);
			single_request.mutable_options()->CopyFrom(request.options());
			callback(getRoute(single_request));
		}
	}
}
