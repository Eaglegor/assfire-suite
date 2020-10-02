set(TARGET_NAME assfire-sample-engine)
set(SOURCES
        Greeter.cpp
        MetricsCollector.cpp)
set(HEADERS
        Greeter.hpp
        assfire/sample/MetricsCollector.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)

add_library(${TARGET_NAME} ${SOURCES})
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog prometheus-cpp::core)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})