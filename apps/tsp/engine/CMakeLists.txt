set(SOURCES
        assfire/tsp/MetricsCollector.cpp)
set(HEADERS
        assfire/tsp/MetricsCollector.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)

add_library(assfire-tsp-engine ${SOURCES})
target_link_libraries(assfire-tsp-engine PUBLIC spdlog::spdlog prometheus-cpp::core)
target_include_directories(assfire-tsp-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})