#include <gtest/gtest.h>
#include <assfire/router/api/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp>
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include <memory>

using namespace assfire;
using namespace assfire::router;
using namespace assfire::locations;

class DirectRequestDistanceMatrixEngineTest : public ::testing::Test {
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

        result->addResponse(10, 10, {getLocation(10, 10), getLocation(20, 20), getLocation(30, 30)});
        result->addResponse(20, 20, {getLocation(40, 40), getLocation(50, 50), getLocation(60, 60)});
        result->addResponse(30, 30, {getLocation(70, 70), getLocation(80, 80), getLocation(90, 90)});
        result->addResponse(40, 40, {getLocation(100, 100), getLocation(110, 110), getLocation(120, 120)});
        result->addResponse(50, 50, {getLocation(130, 130), getLocation(140, 140), getLocation(150, 150)});
        result->addResponse(60, 60, {getLocation(160, 160), getLocation(170, 170), getLocation(180, 180)});
        result->addResponse(70, 70, {getLocation(190, 190), getLocation(200, 200), getLocation(210, 210)});

        return result;
    }
};

TEST_F(DirectRequestDistanceMatrixEngineTest, ReturnsRouteInfoFromProvider)
{
    DirectRequestDistanceMatrixEngine engine(createMockRouteProviderEngine(), DistanceMatrixEngine::Tag(0));

    Location origin = getOrigin();
    Location destination = getDestination();
    IndexedLocation indexedOrigin = engine.addLocation(origin, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN);
    IndexedLocation indexedDestination = engine.addLocation(destination, assfire::router::DistanceMatrixEngine::LocationType::DESTINATION);

    RouteInfo response1 = engine.getRouteInfo(origin, destination);
    RouteInfo response2 = engine.getRouteInfo(indexedOrigin, indexedDestination);
    RouteInfo response3 = engine.getRouteInfo(indexedOrigin, indexedDestination);

    ASSERT_EQ(response1.getDistance().toMeters(), 10);
    ASSERT_EQ(response1.getDuration().toSeconds(), 10);

    ASSERT_EQ(response2.getDistance().toMeters(), 20);
    ASSERT_EQ(response2.getDuration().toSeconds(), 20);

    ASSERT_EQ(response3.getDistance().toMeters(), 30);
    ASSERT_EQ(response3.getDuration().toSeconds(), 30);
}

TEST_F(DirectRequestDistanceMatrixEngineTest, ReturnsRouteDetailsFromProvider)
{
    DirectRequestDistanceMatrixEngine engine(createMockRouteProviderEngine(), DistanceMatrixEngine::Tag(0));

    Location origin = getOrigin();
    Location destination = getDestination();
    IndexedLocation indexedOrigin = engine.addLocation(origin, assfire::router::DistanceMatrixEngine::LocationType::ORIGIN);
    IndexedLocation indexedDestination = engine.addLocation(destination, assfire::router::DistanceMatrixEngine::LocationType::DESTINATION);

    RouteDetails response1 = engine.getRouteDetails(origin, destination);
    RouteDetails response2 = engine.getRouteDetails(indexedOrigin, indexedDestination);
    RouteDetails response3 = engine.getRouteDetails(indexedOrigin, indexedDestination);

    ASSERT_EQ(response1.getDistance().toMeters(), 10);
    ASSERT_EQ(response1.getDuration().toSeconds(), 10);
    ASSERT_EQ(response1.getWaypoints().size(), 3);
    ASSERT_DOUBLE_EQ(response1.getWaypoints()[0].getLatitude().doubleValue(), 10);
    ASSERT_DOUBLE_EQ(response1.getWaypoints()[1].getLatitude().doubleValue(), 20);
    ASSERT_DOUBLE_EQ(response1.getWaypoints()[2].getLatitude().doubleValue(), 30);

    ASSERT_EQ(response2.getDistance().toMeters(), 20);
    ASSERT_EQ(response2.getDuration().toSeconds(), 20);
    ASSERT_EQ(response2.getWaypoints().size(), 3);
    ASSERT_DOUBLE_EQ(response2.getWaypoints()[0].getLatitude().doubleValue(), 40);
    ASSERT_DOUBLE_EQ(response2.getWaypoints()[1].getLatitude().doubleValue(), 50);
    ASSERT_DOUBLE_EQ(response2.getWaypoints()[2].getLatitude().doubleValue(), 60);

    ASSERT_EQ(response3.getDistance().toMeters(), 30);
    ASSERT_EQ(response3.getDuration().toSeconds(), 30);
    ASSERT_EQ(response3.getWaypoints().size(), 3);
    ASSERT_DOUBLE_EQ(response3.getWaypoints()[0].getLatitude().doubleValue(), 70);
    ASSERT_DOUBLE_EQ(response3.getWaypoints()[1].getLatitude().doubleValue(), 80);
    ASSERT_DOUBLE_EQ(response3.getWaypoints()[2].getLatitude().doubleValue(), 90);
}

TEST_F(DirectRequestDistanceMatrixEngineTest, ReturnsInfinityOnErrorWithDefaultErrorPolicy)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    DirectRequestDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0));

    Location origin = getOrigin();
    Location destination = getDestination();

    RouteDetails response1 = engine.getRouteDetails(origin, destination);

    ASSERT_ANY_THROW(backend_ref.getSingleRouteInfo(origin, destination));

    RouteDetails response2 = engine.getRouteDetails(origin, destination);

    ASSERT_TRUE(response2.isInfinity());
}

TEST_F(DirectRequestDistanceMatrixEngineTest, ReturnsInfinityOnErrorWithCorrespondingErrorPolicy)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    DirectRequestDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0), DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY);

    Location origin = getOrigin();
    Location destination = getDestination();

    RouteDetails response1 = engine.getRouteDetails(origin, destination);

    ASSERT_ANY_THROW(backend_ref.getSingleRouteInfo(origin, destination));

    RouteDetails response2 = engine.getRouteDetails(origin, destination);

    ASSERT_TRUE(response2.isInfinity());
}

TEST_F(DirectRequestDistanceMatrixEngineTest, ThrowsOnErrorWithCorrespondingErrorPolicy)
{
    std::unique_ptr<MockRouteProviderEngine> backend_engine = std::make_unique<MockRouteProviderEngine>();
    const auto& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    DirectRequestDistanceMatrixEngine engine(std::move(backend_engine), DistanceMatrixEngine::Tag(0), DistanceMatrixErrorPolicy::ON_ERROR_THROW);

    Location origin = getOrigin();
    Location destination = getDestination();

    RouteDetails response1 = engine.getRouteDetails(origin, destination);

    ASSERT_ANY_THROW(backend_ref.getSingleRouteInfo(origin, destination));

    ASSERT_ANY_THROW(engine.getRouteDetails(origin, destination));
}