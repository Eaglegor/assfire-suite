#include "gtest/gtest.h"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include <assfire/router/engine/impl/route_provider_engines/RedisRouteProviderEngine.hpp>
#include <assfire/router/engine/DefaultRedisSerializer.hpp>
#include "RouteProviderEngineTestFixtureBase.hpp"
#include "MockCacheConnector.hpp"
#include <memory>

using namespace assfire;
using namespace assfire::router;
using namespace assfire::locations;

namespace assfire::router {
    class RedisRouteProviderEngineTest : public RouteProviderEngineTestFixtureBase {
    public:
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

        std::unique_ptr<RedisSerializer> getRedisSerializer() {
            return std::make_unique<DefaultRedisSerializer>(RouterEngineType::RANDOM, getRoutingProfile(60), RouteProviderSettings());
        }

        std::unique_ptr<MockCacheConnector> getCacheConnector() {
            return std::make_unique<MockCacheConnector>(cache);
        }

        std::unique_ptr<MockRouteProviderEngine> getMockEngine() {
            return std::make_unique<MockRouteProviderEngine>();
        }

        std::unique_ptr<RedisRouteProviderEngine> getCachedEngine(std::unique_ptr<MockRouteProviderEngine> mock_engine, bool forceUpdate) {
            return std::make_unique<RedisRouteProviderEngine>(getRoutingProfile(60), std::move(getRedisSerializer()), std::move(mock_engine), std::move(getCacheConnector()), forceUpdate);
        }

    protected:
        void SetUp() override {
            cache = std::make_shared<MockCache>();
        }

    public:
        std::shared_ptr<MockCache> cache;
    };
}

TEST_F(RedisRouteProviderEngineTest, PreCheckMockIsReturningValuesAsExpected) {
    auto backend_engine = getMockEngine();
    backend_engine->addResponse(0, 10);
    backend_engine->addResponse(100, 20);

    RouteInfo result1 = backend_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result1.getDistance().toMeters(), 0);
    ASSERT_EQ(result1.getDuration().toSeconds(), 10);

    RouteInfo result2 = backend_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result2.getDistance().toMeters(), 100);
    ASSERT_EQ(result2.getDuration().toSeconds(), 20);
}

TEST_F(RedisRouteProviderEngineTest, RouteIsRetrievedFromCache) {
    cache->put(getRedisSerializer()->serializeKey(getOrigin(), getDestination()),
               getRedisSerializer()->serializeRouteDetails(getOrigin(), getDestination(), RouteDetails(RouteInfo(Distance::fromMeters(50), TimeInterval::fromSeconds(50)), {})));

    auto backend_engine = getMockEngine();
    backend_engine->addResponse(0, 10);
    backend_engine->addResponse(100, 20);
    auto cached_engine = getCachedEngine(std::move(backend_engine), false);


    RouteInfo result1 = cached_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result1.getDistance().toMeters(), 50);
    ASSERT_EQ(result1.getDuration().toSeconds(), 50);
}

TEST_F(RedisRouteProviderEngineTest, SameResultIsReturnedForRepeatedCallsOnDifferentBackendResponses) {
    auto backend_engine = getMockEngine();
    backend_engine->addResponse(0, 10);
    backend_engine->addResponse(100, 20);
    auto cached_engine = getCachedEngine(std::move(backend_engine), false);

    RouteInfo result1 = cached_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result1.getDistance().toMeters(), 0);
    ASSERT_EQ(result1.getDuration().toSeconds(), 10);

    RouteInfo result2 = cached_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result2.getDistance().toMeters(), 0);
    ASSERT_EQ(result2.getDuration().toSeconds(), 10);
}

TEST_F(RedisRouteProviderEngineTest, WaypointsAreRetrievedFromCache) {
    auto backend_engine = getMockEngine();
    backend_engine->addResponse(0, 10, {getLocation(50, 50), getLocation(10, 10)});
    backend_engine->addResponse(100, 20, {getLocation(30, 10), getLocation(30, 25), getLocation(5, 20)});
    auto cached_engine = getCachedEngine(std::move(backend_engine), false);

    RouteDetails result1 = cached_engine->getSingleRouteDetails(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result1.getDistance().toMeters(), 0);
    ASSERT_EQ(result1.getDuration().toSeconds(), 10);
    ASSERT_EQ(result1.getWaypoints().size(), 2);
    ASSERT_EQ(result1.getWaypoints()[0], getLocation(50, 50));
    ASSERT_EQ(result1.getWaypoints()[1], getLocation(10, 10));

    RouteInfo result2 = cached_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result2.getDistance().toMeters(), 0);
    ASSERT_EQ(result2.getDuration().toSeconds(), 10);
    ASSERT_EQ(result1.getWaypoints().size(), 2);
    ASSERT_EQ(result1.getWaypoints()[0], getLocation(50, 50));
    ASSERT_EQ(result1.getWaypoints()[1], getLocation(10, 10));
}

TEST_F(RedisRouteProviderEngineTest, CacheIsUpdatedOnForceUpdate) {
    auto backend_engine = getMockEngine();
    backend_engine->addResponse(0, 10);
    backend_engine->addResponse(100, 20);
    auto cached_engine = getCachedEngine(std::move(backend_engine), false);

    RouteInfo result1 = cached_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result1.getDistance().toMeters(), 0);
    ASSERT_EQ(result1.getDuration().toSeconds(), 10);

    RouteInfo result2 = cached_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result2.getDistance().toMeters(), 0);
    ASSERT_EQ(result2.getDuration().toSeconds(), 10);

    backend_engine = getMockEngine();
    backend_engine->addResponse(30, 15);
    cached_engine = getCachedEngine(std::move(backend_engine), true);

    RouteInfo result3 = cached_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result3.getDistance().toMeters(), 30);
    ASSERT_EQ(result3.getDuration().toSeconds(), 15);

    backend_engine = getMockEngine();
    backend_engine->addResponse(45, 30);
    cached_engine = getCachedEngine(std::move(backend_engine), false);

    RouteInfo result4 = cached_engine->getSingleRouteInfo(getOrigin(), getDestination());
    ASSERT_DOUBLE_EQ(result4.getDistance().toMeters(), 30);
    ASSERT_EQ(result4.getDuration().toSeconds(), 15);
}

TEST_F(RedisRouteProviderEngineTest, OnlyMissingRoutesAreRequestedAtBackendInMatrixMode) {
    auto backend_engine = getMockEngine();
    const MockRouteProviderEngine& backend_ref = *backend_engine;
    backend_engine->addResponse(10, 10);
    backend_engine->addResponse(20, 20);
    backend_engine->addResponse(30, 30);
    backend_engine->addResponse(40, 40);
    backend_engine->addResponse(50, 50);
    backend_engine->addResponse(60, 60);
    backend_engine->addResponse(70, 70);
    backend_engine->addResponse(80, 80);
    auto cached_engine = getCachedEngine(std::move(backend_engine), false);

    RouteProviderEngine::LocationsList origins{getLocation(1, 1), getLocation(2, 2), getLocation(3, 3)};
    RouteProviderEngine::LocationsList destinations{getLocation(4, 4), getLocation(2, 2), getLocation(5, 5)};

    cached_engine->getSingleRouteInfo(getLocation(1,1), getLocation(5,5)); // (1,1) -> (5,5) = 10
    cached_engine->getSingleRouteInfo(getLocation(3,3), getLocation(2,2)); // (3,3) -> (2,2) = 20
    cached_engine->getSingleRouteInfo(getLocation(2,2), getLocation(4,4)); // (2,2) -> (4,4) = 30

    ASSERT_EQ(backend_ref.getCallsCount(), 3);

    auto matrix = cached_engine->getRouteInfoMatrix(origins, destinations);

    ASSERT_EQ(backend_ref.getCallsCount(), 8); // (1,1) -> (4,4) = 40; (1,1) -> (2,2) = 50; (2,2) -> (5,5) = 60; (3,3) -> (4,4) = 70; (3,3) -> (5,5) = 80;

    // Matrix:
    //    4    2    5
    // 1  40   50   10
    // 2  30   0    60
    // 3  70   20   80

    RouteInfo r11 = cached_engine->getSingleRouteInfo(getLocation(1,1), getLocation(4,4));
    RouteInfo r12 = cached_engine->getSingleRouteInfo(getLocation(1,1), getLocation(2,2));
    RouteInfo r13 = cached_engine->getSingleRouteInfo(getLocation(1,1), getLocation(5,5));

    RouteInfo r21 = cached_engine->getSingleRouteInfo(getLocation(2,2), getLocation(4,4));
    RouteInfo r22 = cached_engine->getSingleRouteInfo(getLocation(2,2), getLocation(2,2));
    RouteInfo r23 = cached_engine->getSingleRouteInfo(getLocation(2,2), getLocation(5,5));

    RouteInfo r31 = cached_engine->getSingleRouteInfo(getLocation(3,3), getLocation(4,4));
    RouteInfo r32 = cached_engine->getSingleRouteInfo(getLocation(3,3), getLocation(2,2));
    RouteInfo r33 = cached_engine->getSingleRouteInfo(getLocation(3,3), getLocation(5,5));

    ASSERT_EQ(r11.getDuration().toSeconds(), 40);
    ASSERT_EQ(r12.getDuration().toSeconds(), 50);
    ASSERT_EQ(r13.getDuration().toSeconds(), 10);

    ASSERT_EQ(r21.getDuration().toSeconds(), 30);
    ASSERT_EQ(r22.getDuration().toSeconds(), 0);
    ASSERT_EQ(r23.getDuration().toSeconds(), 60);

    ASSERT_EQ(r31.getDuration().toSeconds(), 70);
    ASSERT_EQ(r32.getDuration().toSeconds(), 20);
    ASSERT_EQ(r33.getDuration().toSeconds(), 80);
}
