include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_library(assfire-router-test-base
        assfire/router/tests/MockRouteProviderEngine.hpp
        assfire/router/tests/MockRouteProviderEngine.cpp
)
target_link_libraries(assfire-router-test-base PRIVATE GTest::gtest GTest::gtest_main assfire-router-engine assfire-router-api assfire-router-client)
target_include_directories(assfire-router-test-base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(assfire-router-test
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
        engine/MockCache.cpp
        api/DirectRequestDistanceMatrixEngineTest.cpp
        api/FullMatrixCacheDistanceMatrixEngineTest.cpp
        api/DistanceMatrixTest.cpp
        client/MockProtobufClient.hpp
        client/MockProtobufClient.cpp
        client/GrpcRouteProviderEngineTest.cpp)
target_link_libraries(assfire-router-test PRIVATE GTest::gtest GTest::gtest_main assfire-router-engine assfire-router-api assfire-router-client assfire-router-test-base)

gtest_discover_tests(assfire-router-test
        XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/junit_test_report)
