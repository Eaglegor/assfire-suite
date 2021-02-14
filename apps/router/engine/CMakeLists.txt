set(SOURCES
        assfire/engine/router/DistanceMatrixFactory.cpp
        assfire/engine/router/impl/route_provider_engines/CrowflightRouteProviderEngine.cpp
        assfire/engine/router/impl/route_provider_engines/EuclideanRouteProviderEngine.cpp
        assfire/engine/router/impl/route_provider_engines/RandomRouteProviderEngine.cpp
        assfire/engine/router/impl/route_provider_engines/OsrmRouteProviderEngine.cpp
        assfire/engine/router/impl/route_provider_engines/RedisRouteProviderEngine.cpp
        assfire/engine/router/impl/route_provider_engines/RedisRouteProviderEngine.cpp)
set(HEADERS
        assfire/engine/router/MetricsCollector.hpp
        assfire/engine/router/DistanceMatrixFactory.hpp
        assfire/engine/router/impl/route_provider_engines/CrowflightRouteProviderEngine.hpp
        assfire/engine/router/impl/route_provider_engines/EuclideanRouteProviderEngine.hpp
        assfire/engine/router/impl/route_provider_engines/RandomRouteProviderEngine.hpp
        assfire/engine/router/impl/route_provider_engines/OsrmRouteProviderEngine.hpp
        assfire/engine/router/impl/route_provider_engines/RedisRouteProviderEngine.hpp
        assfire/engine/router/impl/route_provider_engines/RedisRouteProviderEngine.hpp
        )

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_library(assfire-router-engine ${SOURCES})
target_link_libraries(assfire-router-engine PUBLIC cpp_redis::cpp_redis spdlog::spdlog prometheus-cpp::core assfire-router-api cpprestsdk::cpprest)
target_include_directories(assfire-router-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})