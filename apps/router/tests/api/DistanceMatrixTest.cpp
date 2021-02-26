#include "gtest/gtest.h"
#include <assfire/router/api/DistanceMatrix.hpp>

using namespace assfire;
using namespace assfire::router;

class MockDistanceMatrixEngine : public DistanceMatrixEngine {
public:
    RouteInfo getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const override {
        return getRouteInfo(origin.getLocation(), destination.getLocation());
    }

    RouteDetails getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const override {
        return getRouteDetails(origin.getLocation(), destination.getLocation());
    }

    RouteInfo getRouteInfo(const Location &origin, const Location &destination) const override {
        ++counter;
        return RouteInfo(Distance::fromMeters(counter), TimeInterval::fromSeconds(counter));
    }

    RouteDetails getRouteDetails(const Location &origin, const Location &destination) const override {
        return RouteDetails(getRouteInfo(origin, destination), {});
    }

    IndexedLocation addLocation(const Location &location, LocationType type) override {
        return IndexedLocation(0, 0, location);
    }

private:
    mutable int counter = 0;
};

TEST(DistanceMatrixTest, MatrixEngineIsSharedBetweenCopies)
{
    DistanceMatrix matrix1(std::make_shared<MockDistanceMatrixEngine>());
    RouteInfo response1 = matrix1.getRouteInfo(Location(), Location());

    DistanceMatrix matrix2(matrix1);
    RouteInfo response2 = matrix1.getRouteInfo(Location(), Location());
    RouteInfo response3 = matrix2.getRouteInfo(Location(), Location());
    RouteInfo response4 = matrix1.getRouteInfo(Location(), Location());

    ASSERT_EQ(response1.getDuration().toSeconds(), 1);
    ASSERT_EQ(response2.getDuration().toSeconds(), 2);
    ASSERT_EQ(response3.getDuration().toSeconds(), 3);
    ASSERT_EQ(response4.getDuration().toSeconds(), 4);
}