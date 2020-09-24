#include <catch2/catch.hpp>
#include "backends/random/RandomRouter.hpp"

using namespace assfire;
using namespace assfire::routing::proto::v1;

TEST_CASE("Random router - get single route")
{
	RandomRouter router;

	GetSingleRouteRequest request;

	request.mutable_from()->set_latitude(0);
	request.mutable_from()->set_longitude(0);

	request.mutable_to()->set_latitude(100);
	request.mutable_to()->set_longitude(100);

    request.mutable_options()->set_routing_type(routing::proto::v1::RoutingOptions::RANDOM);
    request.mutable_options()->mutable_coordinates_format()->set_type(routing::proto::v1::CoordinateFormat::FIXED_POINT_INT);
    request.mutable_options()->set_velocity(16.6);
    request.mutable_options()->mutable_coordinates_format()->set_precision(0);

	RouteInfo route_info1 = router.getRoute(request, 1).route_info();
	RouteInfo route_info2 = router.getRoute(request, 2).route_info();

	REQUIRE(route_info1.distance() != Approx(route_info2.distance()));
	REQUIRE(route_info1.duration() != route_info2.duration());
}

TEST_CASE("Random router - get routes batch")
{
	RandomRouter router;

	GetRoutesBatchRequest request;

	Location* loc0 = request.add_origins();
	loc0->set_latitude(0);
	loc0->set_longitude(0);

	Location* loc1 = request.add_origins();
	loc1->set_latitude(100);
	loc1->set_longitude(100);

	Location* loc2 = request.add_origins();
	loc2->set_latitude(-100);
	loc2->set_longitude(-100);

	request.add_destinations()->CopyFrom(*loc1);
	request.add_destinations()->CopyFrom(*loc2);

    request.mutable_options()->set_routing_type(routing::proto::v1::RoutingOptions::RANDOM);
    request.mutable_options()->mutable_coordinates_format()->set_type(routing::proto::v1::CoordinateFormat::FIXED_POINT_INT);
    request.mutable_options()->set_velocity(16.6);
    request.mutable_options()->mutable_coordinates_format()->set_precision(0);

	std::vector<RouteInfo> results1;
	GetRoutesBatchResponse response1 = router.getRoutesBatch(request, 1);
    for(const RouteInfo& info : response1.route_infos()) {
        results1.push_back(info);
    }
	std::vector<RouteInfo> results2;
    GetRoutesBatchResponse response2 = router.getRoutesBatch(request, 2);
    for(const RouteInfo& info : response2.route_infos()) {
        results2.push_back(info);
    }

	REQUIRE(results1.size() == 6);
	REQUIRE(results2.size() == 6);

	double d1 = std::hypot(100, 100);
	double d2 = std::hypot(200, 200);

	double distances[3][2] = {
		{d1, d1},
		{0, d2},
		{d2, 0}
	};

	auto adjustToSpeed = [](double distance)
	{
		return static_cast<int>(distance / 16.6);
	};

	long durations[3][2] = {
		{adjustToSpeed(distances[0][0]), adjustToSpeed(distances[0][1])},
		{adjustToSpeed(distances[1][0]), adjustToSpeed(distances[1][1])},
		{adjustToSpeed(distances[2][0]), adjustToSpeed(distances[2][1])}
	};

	auto getOriginId = [&](const Location& loc)	{
		for (int i = 0; i < request.origins().size(); ++i) {
			const Location& l = request.origins(i);
			if (l.latitude() == loc.latitude() && l.longitude() == loc.longitude()) return i;
		}
		REQUIRE(false);
	};
	
	auto getDestinationId = [&](const Location& loc) {
		for (int i = 0; i < request.destinations().size(); ++i) {
			const Location& l = request.destinations(i);
			if (l.latitude() == loc.latitude() && l.longitude() == loc.longitude()) return i;
		}
		REQUIRE(false);
	};

	for (int i = 0; i < results1.size(); ++i) {
		const RouteInfo& ri1 = results1[i];
		const RouteInfo& ri2 = results2[i];

		int origin_id = getOriginId(ri1.origin());
		int destination_id = getDestinationId(ri1.destination());

		REQUIRE(ri1.distance() != Approx(ri2.distance()));
		REQUIRE(ri1.duration() != ri2.duration());
	}
}
