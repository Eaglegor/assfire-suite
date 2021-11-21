include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(assfire-tsp-test
        engine/EngineTspSolutionControllerTest.cpp
        )

target_link_libraries(assfire-tsp-test PRIVATE GTest::gtest GTest::gtest_main assfire-tsp-engine assfire-tsp-api assfire-tsp-proto-cpp-translation)

gtest_discover_tests(assfire-tsp-test
        XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/junit_test_report)
