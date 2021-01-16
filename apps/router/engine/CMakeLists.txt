set(SOURCES
        assfire/router/providers/crowflight/CrowflightRouteProvider.cpp
        assfire/router/providers/euclidean/EuclideanRouteProvider.cpp
        assfire/router/providers/random/RandomRouteProvider.cpp
        assfire/router/providers/osrm/HttpOsrmRouteProvider.cpp
        assfire/router/cache/RedisRouteProviderProxy.cpp
        assfire/router/CompositeRouteProvider.cpp
        assfire/router/MetricsCollector.cpp
        assfire/router/impl/distance_matrix_engines/DirectRequestDistanceMatrixEngine.cpp
        )
set(HEADERS
        assfire/router/providers/crowflight/CrowflightRouteProvider.hpp
        assfire/router/providers/euclidean/EuclideanRouteProvider.hpp
        assfire/router/providers/random/RandomRouteProvider.hpp
        assfire/router/providers/osrm/HttpOsrmRouteProvider.hpp
        assfire/router/cache/TransparentRouteProviderProxy.hpp
        assfire/router/cache/RedisRouteProviderProxy.hpp
        assfire/router/RouteProvider.hpp
        assfire/router/MetricsCollector.hpp
        assfire/router/CompositeRouteProvider.hpp
        assfire/router/Router.hpp
        assfire/router/model/RoutingProfile.hpp
        assfire/router/RouterEngineType.hpp
        assfire/router/RouteProviderEngine.hpp
        assfire/router/Matrix.hpp
        assfire/router/DistanceMatrixFactory.hpp
        assfire/router/DistanceMatrixEngine.hpp
        assfire/router/DistanceMatrixCachingPolicy.hpp
        assfire/router/DistanceMatrix.hpp
        assfire/router/impl/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp
        assfire/router/model/Coordinate.hpp
        assfire/router/model/Distance.hpp
        assfire/router/model/IndexedLocation.hpp
        assfire/router/model/Interval.hpp
        assfire/router/model/Location.hpp
        assfire/router/model/RouteDetails.hpp
        assfire/router/model/RouteInfo.hpp
        assfire/router/model/Router.hpp
        assfire/router/model/TimePoint.hpp
        )

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_library(assfire-router-engine ${SOURCES})
target_link_libraries(assfire-router-engine PUBLIC cpp_redis::cpp_redis spdlog::spdlog prometheus-cpp::core assfire-router-api cpprestsdk::cpprest)
target_include_directories(assfire-router-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})