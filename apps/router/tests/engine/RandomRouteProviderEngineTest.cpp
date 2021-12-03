#include "gtest/gtest.h"
#include <assfire/router/engine/impl/route_provider_engines/RandomRouteProviderEngine.hpp>
#include "RouteProviderEngineTestFixtureBase.hpp"
#include <iostream>

using namespace assfire;
using namespace assfire::router;
using namespace assfire::locations;

namespace assfire::router {
    class RandomRouteProviderEngineTest : public RouteProviderEngineTestFixtureBase {
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

        std::vector<Location> getOriginsGroup() {
            return {getLocation(55759, 37618), getLocation(55411, 37897)};
        }

        std::vector<Location> getDestinationsGroup() {
            return {getLocation(52519, 13401), getLocation(55412, 37898), getLocation(50064, 14420)};
        }
    };
}

TEST_F(RandomRouteProviderEngineTest, ReturnsDifferentRouteInfoForRepeatedCall) {
    RandomRouteProviderEngine engine(getRoutingProfile(60));

    RouteInfo result1 = engine.getSingleRouteInfo(getOrigin(), getDestination());
    RouteInfo result2 = engine.getSingleRouteInfo(getOrigin(), getDestination());

    ASSERT_NE(result1.getDistance().toMeters(), result2.getDistance().toMeters());
    ASSERT_NE(result1.getDuration().toSeconds(), result2.getDuration().toSeconds());
}

TEST_F(RandomRouteProviderEngineTest, ReturnsDifferentRouteDetailsForRepeatedCall) {
    RandomRouteProviderEngine engine(getRoutingProfile(60));

    RouteDetails result1 = engine.getSingleRouteDetails(getOrigin(), getDestination());
    RouteDetails result2 = engine.getSingleRouteDetails(getOrigin(), getDestination());

    ASSERT_NE(result1.getSummary().getDistance().toMeters(), result2.getSummary().getDistance().toMeters());
    ASSERT_NE(result1.getSummary().getDuration().toSeconds(), result2.getSummary().getDuration().toSeconds());

    ASSERT_EQ(result1.getWaypoints().size(), 2);
    ASSERT_EQ(result1.getWaypoints()[0], getOrigin());
    ASSERT_EQ(result1.getWaypoints()[1], getDestination());

    ASSERT_EQ(result2.getWaypoints().size(), 2);
    ASSERT_EQ(result2.getWaypoints()[0], getOrigin());
    ASSERT_EQ(result2.getWaypoints()[1], getDestination());
}

TEST_F(RandomRouteProviderEngineTest, ReturnsDifferentRouteInfoMatrixForRepeatedCall) {
    RandomRouteProviderEngine engine(getRoutingProfile(60));

    Matrix<RouteInfo> result1 = engine.getRouteInfoMatrix(getOriginsGroup(), getDestinationsGroup());
    Matrix<RouteInfo> result2 = engine.getRouteInfoMatrix(getOriginsGroup(), getDestinationsGroup());

    ASSERT_EQ(result1.getRowsCount(), getOriginsGroup().size());
    ASSERT_EQ(result1.getColumnsCount(), getDestinationsGroup().size());
    ASSERT_EQ(result2.getRowsCount(), getOriginsGroup().size());
    ASSERT_EQ(result2.getColumnsCount(), getDestinationsGroup().size());

    for(int c = 0; c < result1.getColumnsCount(); ++c) {
        for(int r = 0; r < result1.getRowsCount(); ++r) {
            EXPECT_NE(result1.at(r, c).getDistance().toMeters(), result2.at(r, c).getDistance().toMeters());
            EXPECT_NE(result1.at(r, c).getDuration().toSeconds(), result2.at(r, c).getDuration().toSeconds());
        }
    }
}

TEST_F(RandomRouteProviderEngineTest, ReturnsDifferentRouteDetailsMatrixForRepeatedCall) {
    RandomRouteProviderEngine engine(getRoutingProfile(60));

    std::vector<Location> origins = getOriginsGroup();
    std::vector<Location> destinations = getDestinationsGroup();

    Matrix<RouteDetails> result1 = engine.getRouteDetailsMatrix(getOriginsGroup(), getDestinationsGroup());
    Matrix<RouteDetails> result2 = engine.getRouteDetailsMatrix(getOriginsGroup(), getDestinationsGroup());

    ASSERT_EQ(result1.getRowsCount(), getOriginsGroup().size());
    ASSERT_EQ(result1.getColumnsCount(), getDestinationsGroup().size());
    ASSERT_EQ(result2.getRowsCount(), getOriginsGroup().size());
    ASSERT_EQ(result2.getColumnsCount(), getDestinationsGroup().size());

    for(int c = 0; c < result1.getColumnsCount(); ++c) {
        for(int r = 0; r < result1.getRowsCount(); ++r) {
            EXPECT_NE(result1.at(r, c).getDistance().toMeters(), result2.at(r, c).getDistance().toMeters());
            EXPECT_NE(result1.at(r, c).getDuration().toSeconds(), result2.at(r, c).getDuration().toSeconds());

            EXPECT_EQ(result1.at(r,c).getWaypoints().size(), 2);
            EXPECT_EQ(result1.at(r,c).getWaypoints()[0], origins.at(r));
            EXPECT_EQ(result1.at(r,c).getWaypoints()[1], destinations.at(c));

            EXPECT_EQ(result2.at(r,c).getWaypoints().size(), 2);
            EXPECT_EQ(result2.at(r,c).getWaypoints()[0], origins.at(r));
            EXPECT_EQ(result2.at(r,c).getWaypoints()[1], destinations.at(c));
        }
    }
}

TEST_F(RandomRouteProviderEngineTest, ReturnsZeroRouteInfoForSameLocation) {
    RandomRouteProviderEngine engine(getRoutingProfile(60));

    RouteInfo expected = RouteInfo::zero();

    RouteInfo result1 = engine.getSingleRouteInfo(getLocation(5, 5), getLocation(5, 5));
    RouteInfo result2 = engine.getSingleRouteInfo(getLocation(5, 5), getLocation(5, 5));

    ASSERT_EQ(result1.getDuration(), TimeInterval::zero());
    ASSERT_EQ(result1.getDistance(), Distance::zero());
    ASSERT_EQ(result2.getDuration(), TimeInterval::zero());
    ASSERT_EQ(result2.getDistance(), Distance::zero());
}

TEST_F(RandomRouteProviderEngineTest, ReturnsZeroRouteDetailsForSameLocation) {
    RandomRouteProviderEngine engine(getRoutingProfile(60));

    RouteInfo expected = RouteInfo::zero();

    RouteDetails result1 = engine.getSingleRouteDetails(getLocation(5, 5), getLocation(5, 5));
    RouteDetails result2 = engine.getSingleRouteDetails(getLocation(5, 5), getLocation(5, 5));

    ASSERT_EQ(result1.getDuration(), TimeInterval::zero());
    ASSERT_EQ(result1.getDistance(), Distance::zero());
    ASSERT_EQ(result2.getDuration(), TimeInterval::zero());
    ASSERT_EQ(result2.getDistance(), Distance::zero());

    ASSERT_EQ(result1.getWaypoints().size(), 2);
    ASSERT_EQ(result1.getWaypoints()[0], getLocation(5, 5));
    ASSERT_EQ(result1.getWaypoints()[0], getLocation(5, 5));

    ASSERT_EQ(result2.getWaypoints().size(), 2);
    ASSERT_EQ(result2.getWaypoints()[0], getLocation(5, 5));
    ASSERT_EQ(result2.getWaypoints()[0], getLocation(5, 5));
}