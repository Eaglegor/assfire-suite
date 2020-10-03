set(SOURCES
        Greeter.cpp
        MetricsCollector.cpp)
set(HEADERS
        Greeter.hpp
        assfire/sample/MetricsCollector.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)

add_library(assfire-sample-engine ${SOURCES})
target_link_libraries(assfire-sample-engine PUBLIC spdlog::spdlog prometheus-cpp::core)
target_include_directories(assfire-sample-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})