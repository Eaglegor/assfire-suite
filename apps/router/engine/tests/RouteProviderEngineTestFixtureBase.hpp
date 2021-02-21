#pragma once
#include <gtest/gtest.h>

namespace assfire::router {
class RouteProviderEngineTestFixtureBase : public testing::Test {
    public:
        RoutingProfile getRoutingProfile(double speed) const {
            return RoutingProfile(Speed::fromKilometersPerHour(60));
        }

        Location getLocation(double lat, double lon) const {
            return Location(Coordinate::fromDoubleValue(lat), Coordinate::fromDoubleValue(lon));
        }
    };
}