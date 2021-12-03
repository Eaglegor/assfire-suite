#include "gtest/gtest.h"
#include <iostream>
#include <assfire/router/engine/impl/route_provider_engines/OsrmRouteProviderEngine.hpp>
#include "RouteProviderEngineTestFixtureBase.hpp"
#include "OsrmReplies.hpp"
#include "MockOsrmConnector.hpp"

using namespace assfire;
using namespace assfire::router;
using namespace assfire::locations;

namespace assfire::router {
    class OsrmRouteProviderEngineTest : public RouteProviderEngineTestFixtureBase {
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
            return Distance::fromMeters(100000);
        }

        TimeInterval getExpectedDuration() {
            return TimeInterval::fromSeconds(10000);
        }

        std::vector<Location> getOriginsGroup() {
            return {getLocation(55.759, 37.618), getLocation(55.411, 37.897)};
        }

        std::vector<Location> getDestinationsGroup() {
            return {getLocation(52.519, 13.401), getLocation(55.411, 37.897), getLocation(50.064, 14.420)};
        }

        Matrix<double> getExpectedDistancesMatrix() {
            std::vector<std::vector<double>> data{
                    {108, 111, 105},
                    {107, 0,     105}
            };

            return Matrix<double>(2, 3, [&](int i, int j) {
                return data[i][j];
            });
        }

        Matrix<long> getExpectedDurationsMatrix() {
            std::vector<std::vector<long>> data{
                    {51019000, 75515000, 52038000},
                    {51298000, 0,    52317000}
            };

            return Matrix<long>(2, 3, [&](int i, int j) {
                return data[i][j];
            });
        }

    protected:
        void SetUp() override {
            connector = std::make_unique<MockOsrmConnector>();

            MockOsrmConnector &mock_connector = static_cast<MockOsrmConnector &>(*connector);

            {
                mock_connector.addReply(
                        test::osrm_replies::buildRequest(getOrigin(), getDestination(), "false"),
                        test::osrm_replies::buildResponse(
                                getOrigin(),
                                getDestination(),
                                RouteDetails(
                                        RouteInfo(getExpectedDistance(), getExpectedDuration()),
                                        {
                                                getOrigin(),
                                                getDestination()
                                        }
                                )
                        )
                );
            }

            for (const Location &origin : getOriginsGroup()) {
                for (const Location &destination : getDestinationsGroup()) {
                    mock_connector.addReply(
                            test::osrm_replies::buildRequest(origin, destination, "false"),
                            test::osrm_replies::buildResponse(
                                    origin,
                                    destination,
                                    RouteDetails(
                                            RouteInfo(
                                                    Distance::fromMeters(origin.getLatitude().doubleValue() + destination.getLatitude().doubleValue()),
                                                    TimeInterval::fromSeconds(origin.getLongitude().encodedValue() + destination.getLongitude().encodedValue())
                                            ),
                                            {
                                                    origin,
                                                    destination
                                            }
                                    )
                            )
                    );
                }
            }

        }

    public:

        std::unique_ptr<OsrmConnector> connector;
    };
}

TEST_F(OsrmRouteProviderEngineTest, ReturnsRouteInfo) {
    OsrmRouteProviderEngine engine(getRoutingProfile(60), OsrmGeometry::STRAIGHT_LINE, std::move(connector));

    RouteInfo result = engine.getSingleRouteInfo(getOrigin(), getDestination());

    ASSERT_NEAR(result.getDistance().toMeters(), getExpectedDistance().toMeters(), getDistancePrecision());
    ASSERT_EQ(result.getDuration().toSeconds(), getExpectedDuration().toSeconds());
}


TEST_F(OsrmRouteProviderEngineTest, ReturnsRouteDetails) {
    OsrmRouteProviderEngine engine(getRoutingProfile(60), OsrmGeometry::STRAIGHT_LINE, std::move(connector));

    RouteDetails result = engine.getSingleRouteDetails(getOrigin(), getDestination());

    ASSERT_NEAR(result.getSummary().getDistance().toMeters(), getExpectedDistance().toMeters(), getDistancePrecision());
    ASSERT_EQ(result.getSummary().getDuration().toSeconds(), getExpectedDuration().toSeconds());

    ASSERT_EQ(result.getWaypoints().size(), 2);
    ASSERT_EQ(result.getWaypoints()[0], getOrigin());
    ASSERT_EQ(result.getWaypoints()[1], getDestination());
}

TEST_F(OsrmRouteProviderEngineTest, ReturnsRouteInfoMatrix) {
    OsrmRouteProviderEngine engine(getRoutingProfile(60), OsrmGeometry::STRAIGHT_LINE, std::move(connector));

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

TEST_F(OsrmRouteProviderEngineTest, ReturnsRouteDetailsMatrix) {
    OsrmRouteProviderEngine engine(getRoutingProfile(60), OsrmGeometry::STRAIGHT_LINE, std::move(connector));

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
//            EXPECT_EQ(result.at(r,c).getWaypoints()[0], origins.at(r));
//            EXPECT_EQ(result.at(r,c).getWaypoints()[1], destinations.at(c));
        }
    }
}