#include <catch2/catch.hpp>
#include <assfire_router/client.hpp>
#include <vector>

using namespace assfire;

TEST_CASE("Client requests random distance matrix with L2 caching and without L1 caching")
{
	RoutingClient client;

	RouteRequestSettings settings;
	settings.coordinates_format = FixedPointIntCoordinatesFormat{ 0 };
	settings.calculation_settings = RandomSettings();

	DistanceMatrix distanceMatrix = client.getDistanceMatrix<DirectRequestBackend>({ Location(0, 0), Location(0, 1) }, settings, "localhost", 50051);

	REQUIRE(distanceMatrix.getId(Location(0, 0)) == 0);
	REQUIRE(distanceMatrix.getId(Location(0, 1)) == 1);

	RouteInfo route_info1 = distanceMatrix.getRoute(0, 1);
	RouteInfo route_info2 = distanceMatrix.getRoute(0, 1);

	REQUIRE(route_info1.getDistance() == Approx(route_info2.getDistance()));
	REQUIRE(route_info1.getDuration() == route_info2.getDuration());
}