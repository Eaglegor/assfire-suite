#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <assfire/router/api/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.hpp>
#include "assfire/router/tests/MockRouteProviderEngine.hpp"

using namespace assfire;
using namespace assfire::router;
using namespace assfire::locations;

class FullMatrixCacheDistanceMatrixEngineTest : public ::testing::Test {
public:
    Location getOrigin() {
        return getLocation(55.751, 37.616);
    }

    Location getDestination() {
        return getLocation(59.939, 30.314);
    }

    Location getLocation(double lat, double lon) const {
        return Location::fromDoubleLatLon(lat, lon);
    }

    std::unique_ptr<MockRouteProviderEngine> createMockRouteProviderEngine() {
        std::unique_ptr<MockRouteProviderEngine> result = std::make_unique<MockRouteProviderEngine>();

        for (int i = 1; i <= 20; ++i) {
            result->addResponse(i * 10, i * 10, {getLocation(30 * (i - 1) + 10, 30 * (i - 1) + 10), getLocation(30 * (i - 1) + 20, 30 * (i - 1) + 20), getLocation(30 * (i - 1) + 30, 30 * (i - 1) + 30)});
        }

        return result;
    }
};

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, ReturnsRouteInfoFromCacheForKnownLocation) {
    auto mocked_engine = createMockRouteProviderEngine();
    const auto& mocked_engine_ref = *mocked_engine;
    FullMatrixCacheDistanceMatrixEngine engine(std::move(mocked_engine), DistanceMatrixEngine::Tag(0));

    IndexedLocation indexed_loc1 = engine.addLocation(getOrigin(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    IndexedLocation indexed_loc2 = engine.addLocation(getDestination(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    RouteInfo response1 = engine.getRouteInfo(getOrigin(), getDestination());
    RouteInfo response2 = engine.getRouteInfo(indexed_loc1, indexed_loc2);
    RouteInfo response3 = engine.getRouteInfo(getDestination(), getOrigin());
    RouteInfo response4 = engine.getRouteInfo(indexed_loc2, indexed_loc1);
    RouteInfo response5 = engine.getRouteInfo(getLocation(20, 20), getLocation(30, 30));

    ASSERT_EQ(response1.getDistance().toMeters(), 10);
    ASSERT_EQ(response1.getDuration().toSeconds(), 10);

    ASSERT_EQ(response2.getDistance().toMeters(), 10);
    ASSERT_EQ(response2.getDuration().toSeconds(), 10);

    ASSERT_EQ(response3.getDistance().toMeters(), 20);
    ASSERT_EQ(response3.getDuration().toSeconds(), 20);

    ASSERT_EQ(response4.getDistance().toMeters(), 20);
    ASSERT_EQ(response4.getDuration().toSeconds(), 20);

    ASSERT_EQ(response5.getDistance().toMeters(), 30);
    ASSERT_EQ(response5.getDuration().toSeconds(), 30);

    ASSERT_EQ(mocked_engine_ref.getCallsCount(), 5);
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, ReturnsRouteDetailsFromCacheForKnownLocation) {
    FullMatrixCacheDistanceMatrixEngine engine(createMockRouteProviderEngine(), DistanceMatrixEngine::Tag(0));

    IndexedLocation indexed_loc1 = engine.addLocation(getOrigin(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    IndexedLocation indexed_loc2 = engine.addLocation(getDestination(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    RouteDetails response1 = engine.getRouteDetails(getOrigin(), getDestination());
    RouteDetails response2 = engine.getRouteDetails(indexed_loc1, indexed_loc2);
    RouteDetails response3 = engine.getRouteDetails(getDestination(), getOrigin());
    RouteDetails response4 = engine.getRouteDetails(indexed_loc2, indexed_loc1);
    RouteDetails response5 = engine.getRouteDetails(getLocation(20, 20), getLocation(30, 30));

    ASSERT_EQ(response1.getDistance().toMeters(), 10);
    ASSERT_EQ(response1.getDuration().toSeconds(), 10);
    ASSERT_EQ(response1.getWaypoints().size(), 3);
    ASSERT_DOUBLE_EQ(response1.getWaypoints()[0].getLatitude().doubleValue(), 10);
    ASSERT_DOUBLE_EQ(response1.getWaypoints()[1].getLatitude().doubleValue(), 20);
    ASSERT_DOUBLE_EQ(response1.getWaypoints()[2].getLatitude().doubleValue(), 30);

    ASSERT_EQ(response2.getDistance().toMeters(), 10);
    ASSERT_EQ(response2.getDuration().toSeconds(), 10);
    ASSERT_EQ(response2.getWaypoints().size(), 3);
    ASSERT_DOUBLE_EQ(response2.getWaypoints()[0].getLatitude().doubleValue(), 10);
    ASSERT_DOUBLE_EQ(response2.getWaypoints()[1].getLatitude().doubleValue(), 20);
    ASSERT_DOUBLE_EQ(response2.getWaypoints()[2].getLatitude().doubleValue(), 30);

    ASSERT_EQ(response3.getDistance().toMeters(), 20);
    ASSERT_EQ(response3.getDuration().toSeconds(), 20);
    ASSERT_EQ(response3.getWaypoints().size(), 3);
    ASSERT_DOUBLE_EQ(response3.getWaypoints()[0].getLatitude().doubleValue(), 40);
    ASSERT_DOUBLE_EQ(response3.getWaypoints()[1].getLatitude().doubleValue(), 50);
    ASSERT_DOUBLE_EQ(response3.getWaypoints()[2].getLatitude().doubleValue(), 60);

    ASSERT_EQ(response4.getDistance().toMeters(), 20);
    ASSERT_EQ(response4.getDuration().toSeconds(), 20);
    ASSERT_EQ(response4.getWaypoints().size(), 3);
    ASSERT_DOUBLE_EQ(response4.getWaypoints()[0].getLatitude().doubleValue(), 40);
    ASSERT_DOUBLE_EQ(response4.getWaypoints()[1].getLatitude().doubleValue(), 50);
    ASSERT_DOUBLE_EQ(response4.getWaypoints()[2].getLatitude().doubleValue(), 60);

    ASSERT_EQ(response5.getDistance().toMeters(), 30);
    ASSERT_EQ(response5.getDuration().toSeconds(), 30);
    ASSERT_EQ(response5.getWaypoints().size(), 3);
    ASSERT_DOUBLE_EQ(response5.getWaypoints()[0].getLatitude().doubleValue(), 70);
    ASSERT_DOUBLE_EQ(response5.getWaypoints()[1].getLatitude().doubleValue(), 80);
    ASSERT_DOUBLE_EQ(response5.getWaypoints()[2].getLatitude().doubleValue(), 90);
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, CacheIsUpdatedOnNewKnownLocationAdded) {
    spdlog::set_level(spdlog::level::trace);
    auto mocked_engine = createMockRouteProviderEngine();
    const auto& mocked_engine_ref = *mocked_engine;
    FullMatrixCacheDistanceMatrixEngine engine(std::move(mocked_engine), DistanceMatrixEngine::Tag(0));

    IndexedLocation indexed_loc1 = engine.addLocation(getOrigin(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    IndexedLocation indexed_loc2 = engine.addLocation(getDestination(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    Location unknown_location1 = getLocation(20, 20);
    Location unknown_location2 = getLocation(30, 30);

//    Expected cache state:
//    0    10
//    20   0

    RouteInfo response1 = engine.getRouteInfo(getOrigin(), getDestination());
    RouteInfo response2 = engine.getRouteInfo(indexed_loc1, indexed_loc2);
    RouteInfo response3 = engine.getRouteInfo(unknown_location1, unknown_location2);

    ASSERT_EQ(response1.getDistance().toMeters(), 10);
    ASSERT_EQ(response1.getDuration().toSeconds(), 10);

    ASSERT_EQ(response2.getDistance().toMeters(), 10);
    ASSERT_EQ(response2.getDuration().toSeconds(), 10);

    ASSERT_EQ(response3.getDistance().toMeters(), 30);
    ASSERT_EQ(response3.getDuration().toSeconds(), 30);

    ASSERT_EQ(mocked_engine_ref.getCallsCount(), 5);

    IndexedLocation indexed_loc3 = engine.addLocation(unknown_location1, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    IndexedLocation indexed_loc4 = engine.addLocation(unknown_location2, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    RouteInfo response4 = engine.getRouteInfo(getOrigin(), getDestination());
    RouteInfo response5 = engine.getRouteInfo(indexed_loc1, indexed_loc2);
    RouteInfo response6 = engine.getRouteInfo(unknown_location1, unknown_location2);
    RouteInfo response7 = engine.getRouteInfo(indexed_loc3, indexed_loc4);

//    Expected cache state (30 is already used, 2x2 initial cache matrix must remain the same):
//    0   10  100  110
//    20  0   120  130
//    40  50  0    60
//    70  80  90   0


    ASSERT_EQ(response4.getDistance().toMeters(), 10);
    ASSERT_EQ(response4.getDuration().toSeconds(), 10);

    ASSERT_EQ(response5.getDistance().toMeters(), 10);
    ASSERT_EQ(response5.getDuration().toSeconds(), 10);

    ASSERT_EQ(response6.getDistance().toMeters(), 60);
    ASSERT_EQ(response6.getDuration().toSeconds(), 60);

    ASSERT_EQ(response7.getDistance().toMeters(), 60);
    ASSERT_EQ(response7.getDuration().toSeconds(), 60);

    ASSERT_EQ(mocked_engine_ref.getCallsCount(), 17);
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, CacheIsNotUpdatedOnRepeatedUnknownLocationRequests) {
    FullMatrixCacheDistanceMatrixEngine engine(createMockRouteProviderEngine(), DistanceMatrixEngine::Tag(0));

    IndexedLocation indexed_loc1 = engine.addLocation(getOrigin(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    IndexedLocation indexed_loc2 = engine.addLocation(getDestination(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    Location unknown_location1 = getLocation(20, 20);
    Location unknown_location2 = getLocation(30, 30);

//    Expected cache state:
//    0    10
//    20   0

    RouteInfo response1 = engine.getRouteInfo(getOrigin(), getDestination());
    RouteInfo response2 = engine.getRouteInfo(indexed_loc1, indexed_loc2);
    RouteInfo response3 = engine.getRouteInfo(unknown_location1, unknown_location2);

    ASSERT_EQ(response1.getDistance().toMeters(), 10);
    ASSERT_EQ(response1.getDuration().toSeconds(), 10);

    ASSERT_EQ(response2.getDistance().toMeters(), 10);
    ASSERT_EQ(response2.getDuration().toSeconds(), 10);

    ASSERT_EQ(response3.getDistance().toMeters(), 30);
    ASSERT_EQ(response3.getDuration().toSeconds(), 30);

    RouteInfo response4 = engine.getRouteInfo(getOrigin(), getDestination());
    RouteInfo response5 = engine.getRouteInfo(indexed_loc1, indexed_loc2);
    RouteInfo response6 = engine.getRouteInfo(unknown_location1, unknown_location2);

//    Expected cache state:
//    0    10
//    20   0

    ASSERT_EQ(response4.getDistance().toMeters(), 10);
    ASSERT_EQ(response4.getDuration().toSeconds(), 10);

    ASSERT_EQ(response5.getDistance().toMeters(), 10);
    ASSERT_EQ(response5.getDuration().toSeconds(), 10);

    ASSERT_EQ(response6.getDistance().toMeters(), 40);
    ASSERT_EQ(response6.getDuration().toSeconds(), 40);
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, CacheIsNotUsedIfAnyLocationInRequestIsUnknown) {
    FullMatrixCacheDistanceMatrixEngine engine(createMockRouteProviderEngine(), DistanceMatrixEngine::Tag(0));

    IndexedLocation indexed_loc1 = engine.addLocation(getOrigin(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    IndexedLocation indexed_loc2 = engine.addLocation(getDestination(), assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    Location unknown_location = getLocation(20, 20);

//    Expected cache state:
//    0    10
//    20   0

    RouteInfo response1 = engine.getRouteInfo(getOrigin(), getDestination());
    RouteInfo response2 = engine.getRouteInfo(indexed_loc1, indexed_loc2);
    RouteInfo response3 = engine.getRouteInfo(getOrigin(), unknown_location);
    RouteInfo response4 = engine.getRouteInfo(getOrigin(), unknown_location);
    RouteInfo response5 = engine.getRouteInfo(unknown_location, getDestination());
    RouteInfo response6 = engine.getRouteInfo(unknown_location, getDestination());

    ASSERT_EQ(response1.getDistance().toMeters(), 10);
    ASSERT_EQ(response1.getDuration().toSeconds(), 10);

    ASSERT_EQ(response2.getDistance().toMeters(), 10);
    ASSERT_EQ(response2.getDuration().toSeconds(), 10);

    ASSERT_EQ(response3.getDistance().toMeters(), 30);
    ASSERT_EQ(response3.getDuration().toSeconds(), 30);

    ASSERT_EQ(response4.getDistance().toMeters(), 40);
    ASSERT_EQ(response4.getDuration().toSeconds(), 40);

    ASSERT_EQ(response5.getDistance().toMeters(), 50);
    ASSERT_EQ(response5.getDuration().toSeconds(), 50);

    ASSERT_EQ(response6.getDistance().toMeters(), 60);
    ASSERT_EQ(response6.getDuration().toSeconds(), 60);
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, ReturnsInfinityOnErrorWithDefaultErrorPolicyForUnknownLocation)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    FullMatrixCacheDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0));

    Location origin = getOrigin();
    Location destination = getDestination();

    RouteDetails response1 = engine.getRouteDetails(origin, destination);

    ASSERT_ANY_THROW(backend_ref.getSingleRouteInfo(origin, destination));

    RouteDetails response2 = engine.getRouteDetails(origin, destination);

    ASSERT_TRUE(response2.isInfinity());
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, ReturnsInfinityOnErrorWithCorrespondingErrorPolicyForUnknownLocation)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    FullMatrixCacheDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0), DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY);

    Location origin = getOrigin();
    Location destination = getDestination();

    RouteDetails response1 = engine.getRouteDetails(origin, destination);

    ASSERT_ANY_THROW(backend_ref.getSingleRouteInfo(origin, destination));

    RouteDetails response2 = engine.getRouteDetails(origin, destination);

    ASSERT_TRUE(response2.isInfinity());
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, ThrowsOnErrorWithCorrespondingErrorPolicyForUnknownLocation)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    FullMatrixCacheDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0), DistanceMatrixErrorPolicy::ON_ERROR_THROW);

    Location origin = getOrigin();
    Location destination = getDestination();

    RouteDetails response1 = engine.getRouteDetails(origin, destination);

    ASSERT_ANY_THROW(backend_ref.getSingleRouteInfo(origin, destination));

    ASSERT_ANY_THROW(engine.getRouteDetails(origin, destination));
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, DISABLED_ReturnsInfinityOnErrorWithDefaultErrorPolicyForKnownLocation)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    FullMatrixCacheDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0));

    Location origin = getOrigin();
    Location destination = getDestination();

    engine.addLocation(origin, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    engine.addLocation(destination, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    RouteDetails response1 = engine.getRouteDetails(origin, destination); // Succeeds
    RouteDetails response2 = engine.getRouteDetails(origin, origin); // Succeeds as it's 0
    RouteDetails response3 = engine.getRouteDetails(destination, origin); // Fails and is initialized to infinity
    RouteDetails response4 = engine.getRouteDetails(destination, destination); // Succeeds as it's 0

    ASSERT_EQ(response1.getDuration().toSeconds(), 10);
    ASSERT_TRUE(response2.isZero());
    ASSERT_TRUE(response3.isInfinity());
    ASSERT_TRUE(response4.isZero());
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, DISABLED_ReturnsInfinityOnErrorWithCorrespondingErrorPolicyForKnownLocation)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    FullMatrixCacheDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0), assfire::router::DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY);

    Location origin = getOrigin();
    Location destination = getDestination();

    engine.addLocation(origin, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    engine.addLocation(destination, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    RouteDetails response1 = engine.getRouteDetails(origin, destination); // Succeeds
    RouteDetails response2 = engine.getRouteDetails(origin, origin); // Succeeds as it's 0
    RouteDetails response3 = engine.getRouteDetails(destination, origin); // Fails and is initialized to infinity
    RouteDetails response4 = engine.getRouteDetails(destination, destination); // Succeeds as it's 0

    ASSERT_EQ(response1.getDuration().toSeconds(), 10);
    ASSERT_TRUE(response2.isZero());
    ASSERT_TRUE(response3.isInfinity());
    ASSERT_TRUE(response4.isZero());
}

TEST_F(FullMatrixCacheDistanceMatrixEngineTest, ThrowsOnErrorWithCorrespondingErrorPolicyForKnownLocation)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    FullMatrixCacheDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0), assfire::router::DistanceMatrixErrorPolicy::ON_ERROR_THROW);

    Location origin = getOrigin();
    Location destination = getDestination();

    engine.addLocation(origin, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);
    engine.addLocation(destination, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN_AND_DESTINATION);

    ASSERT_ANY_THROW(engine.getRouteDetails(origin, destination));
}