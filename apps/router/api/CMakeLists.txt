set(SOURCES
        assfire/router/api/distance_matrix_engines/DirectRequestDistanceMatrixEngine.cpp
        assfire/router/api/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.cpp
        assfire/router/api/RoutingContext.hpp
        assfire/router/api/RedisSerializer.hpp)
set(HEADERS
        assfire/router/api/RoutingProfile.hpp
        assfire/router/api/RouterEngineType.hpp
        assfire/router/api/Matrix.hpp
        assfire/router/api/DistanceMatrixEngine.hpp
        assfire/router/api/DistanceMatrixCachingPolicy.hpp
        assfire/router/api/DistanceMatrix.hpp
        assfire/router/api/RouteProviderEngine.hpp
        assfire/router/api/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp
        assfire/router/api/IndexedLocation.hpp
        assfire/router/api/RouteDetails.hpp
        assfire/router/api/RouteInfo.hpp
        assfire/router/api/RouteProviderSettings.hpp
        assfire/router/api/distance_matrix_engines/FullMatrixCacheDistanceMatrixEngine.hpp
        assfire/router/api/DistanceMatrixErrorPolicy.hpp)

find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
include(pkg_utils)
vcpkg_find_cpp_redis()

add_library(assfire-router-api ${SOURCES} ${HEADERS})
target_link_libraries(assfire-router-api PUBLIC assfire-locations-api assfire-concepts-api spdlog::spdlog)
target_include_directories(assfire-router-api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})