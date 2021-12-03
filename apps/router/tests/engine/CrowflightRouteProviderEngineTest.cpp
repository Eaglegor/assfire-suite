#include "gtest/gtest.h"
#include <assfire/router/engine/impl/route_provider_engines/CrowflightRouteProviderEngine.hpp>
#include "RouteProviderEngineTestFixtureBase.hpp"
#include <iostream>

using namespace assfire;
using namespace assfire::router;
using namespace assfire::locations;

namespace assfire::router {
    class CrowflightRouteProviderEngineTest : public RouteProviderEngineTestFixtureBase {
    public:
        double getDistancePrecision() {
            return 1.0;
        }

        Location getOrigin() {
            return getLocation(55.751, 37.616);
        }

        Location getDestination() {
            return getLocation(59.939, 30.314);
        }

        Distance getExpectedDistance() {
            return Distance::fromMeters(637375);
        }

        TimeInterval getExpectedDuration() {
            return TimeInterval::fromSeconds(38242);
        }

        std::vector<Location> getOriginsGroup() {
            return {getLocation(55.759, 37.618), getLocation(55.411, 37.897)};
        }

        std::vector<Location> getDestinationsGroup() {
            return {getLocation(52.519, 13.401), getLocation(55.411, 37.897), getLocation(50.064, 14.420)};
        }

        Matrix<double> getExpectedDistancesMatrix() {
            std::vector<std::vector<double>> data{
                    {1616257, 42669, 1676856},
                    {1632492, 0, 1686184}
            };

            return Matrix<double>(2, 3, [&](int i, int j){
                return data[i][j];
            });
        }

        Matrix<long> getExpectedDurationsMatrix() {
            std::vector<std::vector<long>> data{
                    {96975, 2560, 100611},
                    {97949, 0, 101171}
            };

            return Matrix<long>(2, 3, [&](int i, int j){
                return data[i][j];
            });
        }
    };
}

TEST_F(CrowflightRouteProviderEngineTest, ReturnsRouteInfo) {
    CrowflightRouteProviderEngine engine(getRoutingProfile(60));

    RouteInfo result = engine.getSingleRouteInfo(getOrigin(), getDestination());

    ASSERT_NEAR(result.getDistance().toMeters(), getExpectedDistance().toMeters(), getDistancePrecision());
    ASSERT_EQ(result.getDuration().toSeconds(), getExpectedDuration().toSeconds());
}

TEST_F(CrowflightRouteProviderEngineTest, ReturnsRouteDetails) {
    CrowflightRouteProviderEngine engine(getRoutingProfile(60));

    RouteDetails result = engine.getSingleRouteDetails(getOrigin(), getDestination());

    ASSERT_NEAR(result.getSummary().getDistance().toMeters(), getExpectedDistance().toMeters(), getDistancePrecision());
    ASSERT_EQ(result.getSummary().getDuration().toSeconds(), getExpectedDuration().toSeconds());

    ASSERT_EQ(result.getWaypoints().size(), 2);
    ASSERT_EQ(result.getWaypoints()[0], getOrigin());
    ASSERT_EQ(result.getWaypoints()[1], getDestination());
}

TEST_F(CrowflightRouteProviderEngineTest, ReturnsRouteInfoMatrix) {
    CrowflightRouteProviderEngine engine(getRoutingProfile(60));

    Matrix<RouteInfo> result = engine.getRouteInfoMatrix(getOriginsGroup(), getDestinationsGroup());

    Matrix<double> expected_distances = getExpectedDistancesMatrix();
    Matrix<long> expected_durations = getExpectedDurationsMatrix();

    ASSERT_EQ(result.getRowsCount(), expected_distances.getRowsCount());
    ASSERT_EQ(result.getColumnsCount(), expected_distances.getColumnsCount());

    for(int c = 0; c < result.getColumnsCount(); ++c) {
        for(int r = 0; r < result.getRowsCount(); ++r) {
            EXPECT_NEAR(result.at(r, c).getDistance().toMeters(), expected_distances.at(r, c), getDistancePrecision());
            EXPECT_EQ(result.at(r, c).getDuration().toSeconds(), expected_durations.at(r, c));
        }
    }
}

TEST_F(CrowflightRouteProviderEngineTest, ReturnsRouteDetailsMatrix) {
    CrowflightRouteProviderEngine engine(getRoutingProfile(60));

    std::vector<Location> origins = getOriginsGroup();
    std::vector<Location> destinations = getDestinationsGroup();

    Matrix<RouteDetails> result = engine.getRouteDetailsMatrix(origins, destinations);

    Matrix<double> expected_distances = getExpectedDistancesMatrix();
    Matrix<long> expected_durations = getExpectedDurationsMatrix();

    ASSERT_EQ(result.getRowsCount(), expected_distances.getRowsCount());
    ASSERT_EQ(result.getColumnsCount(), expected_distances.getColumnsCount());

    for(int c = 0; c < result.getColumnsCount(); ++c) {
        for(int r = 0; r < result.getRowsCount(); ++r) {
            EXPECT_NEAR(result.at(r, c).getSummary().getDistance().toMeters(), expected_distances.at(r, c), getDistancePrecision());
            EXPECT_EQ(result.at(r, c).getSummary().getDuration().toSeconds(), expected_durations.at(r, c));

            EXPECT_EQ(result.at(r,c).getWaypoints().size(), 2);
            EXPECT_EQ(result.at(r,c).getWaypoints()[0], origins.at(r));
            EXPECT_EQ(result.at(r,c).getWaypoints()[1], destinations.at(c));
        }
    }
}