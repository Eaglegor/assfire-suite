#include <catch2/catch.hpp>
#include "backends/euclidean/EuclideanRouter.hpp"

using namespace assfire;
using namespace assfire::routing::proto::v1;

TEST_CASE("Euclidean router - get single route")
{
	EuclideanRouter router;

	GetSingleRouteRequest request;

	request.mutable_origin()->set_lat(0);
	request.mutable_destination()->set_lon(0);

	request.mutable_origin()->set_lat(100);
	request.mutable_destination()->set_lon(100);

	request.mutable_options()->set_routing_type(routing::proto::v1::RoutingOptions::EUCLIDEAN);
	request.mutable_options()->set_velocity(16.6);

    request.mutable_options()->mutable_coordinates_format()->set_type(routing::proto::v1::CoordinateFormat::FIXED_POINT_INT);
    request.mutable_options()->mutable_coordinates_format()->set_precision(0);

	RouteInfo route_info = router.getRoute(request, 1).route_info();

	REQUIRE(route_info.distance() == Approx(std::hypot(100, 100)));
	REQUIRE(route_info.duration() == static_cast<int>(std::ceil(std::hypot(100, 100) / 16.6)));

}

TEST_CASE("Euclidean router - get routes batch")
{
	EuclideanRouter router;

	GetRoutesBatchRequest request;

	Location* loc0 = request.add_origins();
	loc0->set_lat(0);
	loc0->set_lon(0);

	Location* loc1 = request.add_origins();
	loc1->set_lat(100);
	loc1->set_lon(100);

	Location* loc2 = request.add_origins();
	loc2->set_lat(-100);
	loc2->set_lon(-100);

	request.add_destinations()->CopyFrom(*loc1);
	request.add_destinations()->CopyFrom(*loc2);

    request.mutable_options()->set_routing_type(routing::proto::v1::RoutingOptions::EUCLIDEAN);
	request.mutable_options()->set_velocity(16.6);

    request.mutable_options()->mutable_coordinates_format()->set_type(routing::proto::v1::CoordinateFormat::FIXED_POINT_INT);
    request.mutable_options()->mutable_coordinates_format()->set_precision(0);

	std::vector<RouteInfo> results;
	GetRoutesBatchResponse response = router.getRoutesBatch(request, 1);
	for(const RouteInfo& info : response.route_infos()) {
	    results.push_back(info);
	}

	REQUIRE(results.size() == 6);

	double d1 = std::hypot(100, 100);
	double d2 = std::hypot(200, 200);

	double distances[3][2] = {
		{d1, d1},
		{0, d2},
		{d2, 0}
	};

	auto adjustToSpeed = [](double distance)
	{
		return static_cast<int>(std::ceil(distance / 16.6));
	};

	long durations[3][2] = {
		{adjustToSpeed(distances[0][0]), adjustToSpeed(distances[0][1])},
		{adjustToSpeed(distances[1][0]), adjustToSpeed(distances[1][1])},
		{adjustToSpeed(distances[2][0]), adjustToSpeed(distances[2][1])}
	};

	auto getOriginId = [&](const Location& loc)	{
		for (int i = 0; i < request.origins().size(); ++i) {
			const Location& l = request.origins(i);
			if (l.lat() == loc.lat() && l.lon() == loc.lon()) return i;
		}
		REQUIRE(false);
	};
	
	auto getDestinationId = [&](const Location& loc) {
		for (int i = 0; i < request.destinations().size(); ++i) {
			const Location& l = request.destinations(i);
			if (l.lat() == loc.lat() && l.lon() == loc.lon()) return i;
		}
		REQUIRE(false);
	};

	for (const RouteInfo& ri : results) {
		int origin_id = getOriginId(ri.origin());
		int destination_id = getDestinationId(ri.destination());

		REQUIRE(ri.distance() == Approx(distances[origin_id][destination_id]));
		REQUIRE(ri.duration() == durations[origin_id][destination_id]);
	}
}
