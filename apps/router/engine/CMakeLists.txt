set(SOURCES
        assfire/router/engine/DistanceMatrixFactory.cpp
        assfire/router/engine/impl/route_provider_engines/CrowflightRouteProviderEngine.cpp
        assfire/router/engine/impl/route_provider_engines/EuclideanRouteProviderEngine.cpp
        assfire/router/engine/impl/route_provider_engines/RandomRouteProviderEngine.cpp
        assfire/router/engine/impl/route_provider_engines/OsrmRouteProviderEngine.cpp
        assfire/router/engine/impl/route_provider_engines/RedisRouteProviderEngine.cpp
        assfire/router/engine/impl/route_provider_engines/RedisRouteProviderEngine.cpp
        assfire/router/engine/impl/route_provider_engines/CppRedisCacheConnector.cpp
        assfire/router/engine/impl/route_provider_engines/RestSdkOsrmConnector.cpp)
set(HEADERS
        assfire/router/engine/MetricsCollector.hpp
        assfire/router/engine/DistanceMatrixFactory.hpp
        assfire/router/engine/impl/route_provider_engines/CrowflightRouteProviderEngine.hpp
        assfire/router/engine/impl/route_provider_engines/EuclideanRouteProviderEngine.hpp
        assfire/router/engine/impl/route_provider_engines/RandomRouteProviderEngine.hpp
        assfire/router/engine/impl/route_provider_engines/OsrmRouteProviderEngine.hpp
        assfire/router/engine/impl/route_provider_engines/RedisRouteProviderEngine.hpp
        assfire/router/engine/impl/route_provider_engines/RedisRouteProviderEngine.hpp
        assfire/router/engine/impl/route_provider_engines/CppRedisCacheConnector.hpp
        assfire/router/engine/impl/route_provider_engines/CacheConnector.hpp
        assfire/router/engine/impl/route_provider_engines/OsrmConnector.hpp
        assfire/router/engine/impl/route_provider_engines/RestSdkOsrmConnector.hpp
        assfire/router/engine/impl/route_provider_engines/CpprestWstringConvert.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_library(assfire-router-engine ${SOURCES})
target_link_libraries(assfire-router-engine PUBLIC cpp_redis::cpp_redis spdlog::spdlog assfire-router-api cpprestsdk::cpprest)
target_include_directories(assfire-router-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
