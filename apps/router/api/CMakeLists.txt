set(SOURCES
        assfire/api/router/distance_matrix_engines/DirectRequestDistanceMatrixEngine.cpp
        assfire/api/router/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.cpp
        assfire/api/router/RoutingContext.hpp assfire/api/router/RedisSerializer.hpp)
set(HEADERS
        assfire/api/router/model/RoutingProfile.hpp
        assfire/api/router/RouterEngineType.hpp
        assfire/api/router/Matrix.hpp
        assfire/api/router/DistanceMatrixEngine.hpp
        assfire/api/router/DistanceMatrixCachingPolicy.hpp
        assfire/api/router/DistanceMatrix.hpp
        assfire/api/router/RouteProviderEngine.hpp
        assfire/api/router/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp
        assfire/api/router/model/Coordinate.hpp
        assfire/api/router/model/Distance.hpp
        assfire/api/router/model/IndexedLocation.hpp
        assfire/api/router/model/TimeInterval.hpp
        assfire/api/router/model/Location.hpp
        assfire/api/router/model/RouteDetails.hpp
        assfire/api/router/model/RouteInfo.hpp
        assfire/api/router/model/Router.hpp
        assfire/api/router/model/TimePoint.hpp
        assfire/api/router/model/Speed.hpp
        assfire/api/router/RouteProviderSettings.hpp
        assfire/api/router/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_library(assfire-router-api ${SOURCES} ${HEADERS})
target_link_libraries(assfire-router-api PUBLIC spdlog::spdlog)
target_include_directories(assfire-router-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})