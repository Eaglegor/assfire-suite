include(GoogleTest)

find_package(GTest CONFIG REQUIRED)
add_executable(assfire-router-engine-test
        engine/RandomRouteProviderEngineTest.cpp
        engine/EuclideanRouteProviderEngineTest.cpp
        engine/CrowflightRouteProviderEngineTest.cpp
        engine/MockCacheConnector.hpp
        engine/MockCacheConnector.cpp
        engine/RouteProviderEngineTestFixtureBase.hpp
        engine/MockOsrmConnector.hpp
        engine/MockOsrmConnector.cpp
        engine/OsrmReplies.hpp
        engine/OsrmRouteProviderEngineTest.cpp
        engine/RedisRouteProviderEngineTest.cpp
        engine/MockRouteProviderEngine.hpp
        engine/MockRouteProviderEngine.cpp
        engine/MockCache.cpp
        api/DirectRequestDistanceMatrixEngineTest.cpp
        api/FullMatrixCacheDistanceMatrixEngineTest.cpp
        api/DistanceMatrixTest.cpp
        client/MockProtobufClient.hpp
        client/MockProtobufClient.cpp)
target_link_libraries(assfire-router-engine-test PRIVATE GTest::gtest GTest::gtest_main assfire-router-engine assfire-router-api assfire-router-client)

gtest_discover_tests(assfire-router-engine-test
        XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/junit_test_report)
