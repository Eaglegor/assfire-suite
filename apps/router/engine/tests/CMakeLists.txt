include(GoogleTest)

find_package(GTest CONFIG REQUIRED)
add_executable(assfire-router-engine-test
        RandomRouteProviderEngineTest.cpp EuclideanRouteProviderEngineTest.cpp CrowflightRouteProviderEngineTest.cpp MockCacheConnector.hpp MockCacheConnector.cpp RouteProviderEngineTestFixtureBase.hpp MockOsrmConnector.hpp MockOsrmConnector.cpp OsrmReplies.hpp OsrmRouteProviderEngineTest.cpp)
target_link_libraries(assfire-router-engine-test PRIVATE GTest::gtest GTest::gtest_main assfire-router-engine)

gtest_discover_tests(assfire-router-engine-test)
