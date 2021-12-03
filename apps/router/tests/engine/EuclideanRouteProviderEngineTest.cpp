#include "gtest/gtest.h"
#include <assfire/router/engine/impl/route_provider_engines/EuclideanRouteProviderEngine.hpp>
#include "RouteProviderEngineTestFixtureBase.hpp"
#include <iostream>

using namespace assfire;
using namespace assfire::router;
using namespace assfire::locations;

namespace assfire::router {
    class EuclideanRouteProviderEngineTest : public RouteProviderEngineTestFixtureBase {
    public:
        double getDistancePrecision() {
            return 1.0;
        }

        Location getOrigin() {
            return getLocation(55751, 37616);
        }

        Location getDestination() {
            return getLocation(59939, 30314);
        }

        Distance getExpectedDistance() {
            return Distance::fromMeters(8417);
        }

        TimeInterval getExpectedDuration() {
            return TimeInterval::fromSeconds(505);
        }

        std::vector<Location> getOriginsGroup() {
            return {getLocation(55759, 37618), getLocation(55411, 37897)};
        }

        std::vector<Location> getDestinationsGroup() {
            return {getLocation(52519, 13401), getLocation(55411, 37897), getLocation(50064, 14420)};
        }

        Matrix<double> getExpectedDistancesMatrix() {
            std::vector<std::vector<double>> data{
                    {24432, 446, 23886},
                    {24666, 0, 24078}
            };

            return Matrix<double>(2, 3, [&](int i, int j){
                return data[i][j];
            });
        }

        Matrix<long> getExpectedDurationsMatrix() {
            std::vector<std::vector<long>> data{
                    {1465, 26, 1433},
                    {1479, 0, 1444}
            };

            return Matrix<long>(2, 3, [&](int i, int j){
                return data[i][j];
            });
        }
    };
}

TEST_F(EuclideanRouteProviderEngineTest, ReturnsRouteInfo) {
    EuclideanRouteProviderEngine engine(getRoutingProfile(60));

    RouteInfo result = engine.getSingleRouteInfo(getOrigin(), getDestination());

    ASSERT_NEAR(result.getDistance().toMeters(), getExpectedDistance().toMeters(), getDistancePrecision());
    ASSERT_EQ(result.getDuration().toSeconds(), getExpectedDuration().toSeconds());
}

TEST_F(EuclideanRouteProviderEngineTest, ReturnsRouteDetails) {
    EuclideanRouteProviderEngine engine(getRoutingProfile(60));

    RouteDetails result = engine.getSingleRouteDetails(getOrigin(), getDestination());

    ASSERT_NEAR(result.getSummary().getDistance().toMeters(), getExpectedDistance().toMeters(), getDistancePrecision());
    ASSERT_EQ(result.getSummary().getDuration().toSeconds(), getExpectedDuration().toSeconds());

    ASSERT_EQ(result.getWaypoints().size(), 2);
    ASSERT_EQ(result.getWaypoints()[0], getOrigin());
    ASSERT_EQ(result.getWaypoints()[1], getDestination());
}

TEST_F(EuclideanRouteProviderEngineTest, ReturnsRouteInfoMatrix) {
    EuclideanRouteProviderEngine engine(getRoutingProfile(60));

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

TEST_F(EuclideanRouteProviderEngineTest, ReturnsRouteDetailsMatrix) {
    EuclideanRouteProviderEngine engine(getRoutingProfile(60));

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