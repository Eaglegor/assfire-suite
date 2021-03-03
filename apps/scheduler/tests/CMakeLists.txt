include(GoogleTest)

find_package(GTest CONFIG REQUIRED)
add_executable(assfire-scheduler-test
        engine/WaybillSchedulingAlgorithmFixtureBase.hpp
        engine/LinearWaybillSchedulingAlgorithmTest.cpp)
target_link_libraries(assfire-scheduler-test PRIVATE GTest::gtest GTest::gtest_main assfire-scheduler-engine assfire-scheduler-api assfire-scheduler-client assfire-router-test-base)

gtest_discover_tests(assfire-scheduler-test
        XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/junit_test_report)
