#pragma once
#include <gtest/gtest.h>
#include "assfire/locations/api/Location.hpp"

namespace assfire::router {
class RouteProviderEngineTestFixtureBase : public testing::Test {
    public:
        RoutingProfile getRoutingProfile(double speed) const {
            return RoutingProfile(Speed::fromKilometersPerHour(60));
        }

        locations::Location getLocation(double lat, double lon) const {
            return locations::Location::fromDoubleLatLon(lat, lon);
        }
    };
}