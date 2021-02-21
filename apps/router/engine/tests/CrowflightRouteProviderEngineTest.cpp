#include "gtest/gtest.h"
#include <assfire/engine/router/impl/route_provider_engines/CrowflightRouteProviderEngine.hpp>
#include "RouteProviderEngineTestFixtureBase.hpp"
#include <iostream>

using namespace assfire::router;

namespace assfire::router {
    class CrowflightRouteProviderEngineTest : public RouteProviderEngineTestFixtureBase {
    public:
        Location getOrigin() {
            return getLocation(55.751, 37.616); // Kremlin
        }

        Location getDestination() {
            return getLocation(59.939, 30.314); // Hermitage
        }

        Distance expectedDistance() {
            return Distance::fromMeters(637375);
        }

        TimeInterval expectedDuration() {
            return TimeInterval::fromSeconds(38242);
        }

        std::vector<Location> getOriginsGroup() {
            return {getOrigin(), getOrigin()};
        }

        std::vector<Location> getDestinationsGroup() {
            return {getDestination(), getDestination(), getDestination()};
        }

        Matrix<double> getExpectedDistancesMatrix() {
            std::vector<std::vector<double>> data{
                    {expectedDistance().toMeters(), expectedDistance().toMeters(), expectedDistance().toMeters()},
                    {expectedDistance().toMeters(), expectedDistance().toMeters(), expectedDistance().toMeters()}
            };

            return Matrix<double>(2, 3, [&](int i, int j){
                return data[i][j];
            });
        }

        Matrix<long> getExpectedDurationsMatrix() {
            std::vector<std::vector<long>> data{
                    {expectedDuration().toSeconds(), expectedDuration().toSeconds(), expectedDuration().toSeconds()},
                    {expectedDuration().toSeconds(), expectedDuration().toSeconds(), expectedDuration().toSeconds()}
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

    ASSERT_NEAR(result.getDistance().toMeters(), expectedDistance().toMeters(), 1.0);
    ASSERT_EQ(result.getDuration().toSeconds(), expectedDuration().toSeconds());
}

TEST_F(CrowflightRouteProviderEngineTest, ReturnsRouteDetails) {
    CrowflightRouteProviderEngine engine(getRoutingProfile(60));

    RouteDetails result = engine.getSingleRouteDetails(getOrigin(), getDestination());

    ASSERT_NEAR(result.getSummary().getDistance().toMeters(), expectedDistance().toMeters(), 1.0);
    ASSERT_EQ(result.getSummary().getDuration().toSeconds(), expectedDuration().toSeconds());

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
            ASSERT_NEAR(result.at(r, c).getDistance().toMeters(), expected_distances.at(r, c), 1.0);
            ASSERT_EQ(result.at(r, c).getDuration().toSeconds(), expected_durations.at(r, c));
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
            ASSERT_NEAR(result.at(r, c).getSummary().getDistance().toMeters(), expected_distances.at(r, c), 1.0);
            ASSERT_EQ(result.at(r, c).getSummary().getDuration().toSeconds(), expected_durations.at(r, c));

            ASSERT_EQ(result.at(r,c).getWaypoints().size(), 2);
            ASSERT_EQ(result.at(r,c).getWaypoints()[0], origins.at(r));
            ASSERT_EQ(result.at(r,c).getWaypoints()[1], destinations.at(c));
        }
    }
}