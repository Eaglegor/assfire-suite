#include "gtest/gtest.h"
#include "MockRouteProviderEngine.hpp"
#include <assfire/engine/router/impl/route_provider_engines/RedisRouteProviderEngine.hpp>
#include "RouteProviderEngineTestFixtureBase.hpp"
#include "MockCacheConnector.hpp"
#include <memory>
#include <assfire/engine/router/DefaultRedisSerializer.hpp>

using namespace assfire::router;

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