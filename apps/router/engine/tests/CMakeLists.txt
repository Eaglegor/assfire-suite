include(GoogleTest)

find_package(GTest CONFIG REQUIRED)
add_executable(assfire-router-engine-test
        RandomRouteProviderEngineTest.cpp
        EuclideanRouteProviderEngineTest.cpp
        CrowflightRouteProviderEngineTest.cpp
        MockCacheConnector.hpp
        MockCacheConnector.cpp
        RouteProviderEngineTestFixtureBase.hpp
        MockOsrmConnector.hpp
        MockOsrmConnector.cpp
        OsrmReplies.hpp
        OsrmRouteProviderEngineTest.cpp
        RedisRouteProviderEngineTest.cpp MockRouteProviderEngine.hpp MockRouteProviderEngine.cpp MockCache.cpp)
target_link_libraries(assfire-router-engine-test PRIVATE GTest::gtest GTest::gtest_main assfire-router-engine)

gtest_discover_tests(assfire-router-engine-test
        XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/junit_test_report)
