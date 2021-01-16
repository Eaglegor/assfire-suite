set(SOURCES
        assfire/engine/router/providers/crowflight/CrowflightRouteProvider.cpp
        assfire/engine/router/providers/euclidean/EuclideanRouteProvider.cpp
        assfire/engine/router/providers/random/RandomRouteProvider.cpp
        assfire/engine/router/providers/osrm/HttpOsrmRouteProvider.cpp
        assfire/engine/router/cache/RedisRouteProviderProxy.cpp
        assfire/engine/router/CompositeRouteProvider.cpp
        assfire/engine/router/MetricsCollector.cpp
        assfire/engine/router/DistanceMatrixFactory.cpp
        assfire/engine/router/impl/distance_matrix_engines/DirectRequestDistanceMatrixEngine.cpp
        assfire/engine/router/impl/route_provider_engines/CrowflightRouteProviderEngine.cpp
        assfire/engine/router/impl/route_provider_engines/EuclideanRouteProviderEngine.cpp
        assfire/engine/router/impl/route_provider_engines/RandomRouteProviderEngine.cpp
        assfire/engine/router/impl/route_provider_engines/OsrmRouteProviderEngine.cpp
        assfire/engine/router/model/Speed.hpp assfire/engine/router/RouteProviderSettings.hpp assfire/engine/router/impl/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.hpp assfire/engine/router/impl/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.cpp)
set(HEADERS
        assfire/engine/router/providers/crowflight/CrowflightRouteProvider.hpp
        assfire/engine/router/providers/euclidean/EuclideanRouteProvider.hpp
        assfire/engine/router/providers/random/RandomRouteProvider.hpp
        assfire/engine/router/providers/osrm/HttpOsrmRouteProvider.hpp
        assfire/engine/router/cache/TransparentRouteProviderProxy.hpp
        assfire/engine/router/cache/RedisRouteProviderProxy.hpp
        assfire/engine/router/RouteProvider.hpp
        assfire/engine/router/MetricsCollector.hpp
        assfire/engine/router/CompositeRouteProvider.hpp
        assfire/engine/router/Router.hpp
        assfire/engine/router/model/RoutingProfile.hpp
        assfire/engine/router/RouterEngineType.hpp
        assfire/engine/router/RouteProviderEngine.hpp
        assfire/engine/router/Matrix.hpp
        assfire/engine/router/DistanceMatrixFactory.hpp
        assfire/engine/router/DistanceMatrixEngine.hpp
        assfire/engine/router/DistanceMatrixCachingPolicy.hpp
        assfire/engine/router/DistanceMatrix.hpp
        assfire/engine/router/impl/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp
        assfire/engine/router/impl/route_provider_engines/CrowflightRouteProviderEngine.hpp
        assfire/engine/router/impl/route_provider_engines/EuclideanRouteProviderEngine.hpp
        assfire/engine/router/impl/route_provider_engines/RandomRouteProviderEngine.hpp
        assfire/engine/router/impl/route_provider_engines/OsrmRouteProviderEngine.hpp
        assfire/engine/router/model/Coordinate.hpp
        assfire/engine/router/model/Distance.hpp
        assfire/engine/router/model/IndexedLocation.hpp
        assfire/engine/router/model/Interval.hpp
        assfire/engine/router/model/Location.hpp
        assfire/engine/router/model/RouteDetails.hpp
        assfire/engine/router/model/RouteInfo.hpp
        assfire/engine/router/model/Router.hpp
        assfire/engine/router/model/TimePoint.hpp
        )

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_library(assfire-router-engine ${SOURCES})
target_link_libraries(assfire-router-engine PUBLIC cpp_redis::cpp_redis spdlog::spdlog prometheus-cpp::core assfire-router-api cpprestsdk::cpprest)
target_include_directories(assfire-router-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})