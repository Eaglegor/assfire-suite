set(SOURCES
        assfire/router/providers/crowflight/CrowflightRouteProvider.cpp
        assfire/router/providers/euclidean/EuclideanRouteProvider.cpp
        assfire/router/providers/random/RandomRouteProvider.cpp
        assfire/router/providers/osrm/HttpOsrmRouteProvider.cpp
        assfire/router/cache/RedisRouteProviderProxy.cpp
        assfire/router/CompositeRouteProvider.cpp
        assfire/router/MetricsCollector.cpp)
set(HEADERS
        assfire/router/providers/crowflight/CrowflightRouteProvider.hpp
        assfire/router/providers/euclidean/EuclideanRouteProvider.hpp
        assfire/router/providers/random/RandomRouteProvider.hpp
        assfire/router/providers/osrm/HttpOsrmRouteProvider.hpp
        assfire/router/cache/TransparentRouteProviderProxy.hpp
        assfire/router/cache/RedisRouteProviderProxy.hpp
        assfire/router/RouteProvider.hpp
        assfire/router/MetricsCollector.hpp
        assfire/router/CompositeRouteProvider.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)

add_library(assfire-router-engine ${SOURCES})
target_link_libraries(assfire-router-engine PUBLIC spdlog::spdlog prometheus-cpp::core assfire-router-api)
target_include_directories(assfire-router-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})