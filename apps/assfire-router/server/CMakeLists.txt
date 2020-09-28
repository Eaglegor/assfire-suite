add_executable(assfire-router main.cpp)

add_library(routes_provider_server_engine
		RoutingService.cpp
		backends/euclidean/EuclideanRouter.cpp
		backends/random/RandomRouter.cpp
		backends/crowflight/CrowflightRouter.cpp
		backends/redis/RedisProxy.cpp
		backends/redis/RouterProxy.hpp
		backends/redis/TransparentProxy.hpp
        RoutingMetricsCollector.cpp
        RoutingMetricsCollector.hpp
        )

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

include(pkg_utils)
vcpkg_find_cpp_redis()

find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

target_link_libraries(routes_provider_server_engine
        PRIVATE cpp_redis::cpp_redis cxxopts::cxxopts spdlog::spdlog prometheus-cpp::core prometheus-cpp::pull
        PUBLIC routes_provider_protobuf_binding)
target_include_directories(routes_provider_server_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(assfire-router PRIVATE routes_provider_server_engine)

include(helm_utils)
prepare_helm_chart(
		CHART_ROOT helm
		VERSION ${ASSFIRE_APPLICATION_RELEASE_VERSION}
)

install(TARGETS assfire-router RUNTIME DESTINATION ${ROUTES_PROVIDER_SERVER_INSTALL_PREFIX})

if (ASSFIRE_AUTO_INSTALL_RUNTIME_DEPS)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake
            CONTENT "
    		file(GET_RUNTIME_DEPENDENCIES
    			RESOLVED_DEPENDENCIES_VAR resolved_deps
    			UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
    			EXECUTABLES $<TARGET_FILE:assfire-router>)
    
    			file(INSTALL FOLLOW_SYMLINK_CHAIN DESTINATION \"${CMAKE_INSTALL_PREFIX}/${ROUTES_PROVIDER_SERVER_INSTALL_PREFIX}\" TYPE EXECUTABLE FILES \${resolved_deps})
    	")
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake)
endif ()

add_subdirectory(tests)