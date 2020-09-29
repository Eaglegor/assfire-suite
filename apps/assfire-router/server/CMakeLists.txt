add_executable(${${PROJECT_PREFIX}_ROUTER_APP_NAME} main.cpp)

add_library(${${PROJECT_PREFIX}_ROUTER_ENGINE_NAME}
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

target_link_libraries(${${PROJECT_PREFIX}_ROUTER_ENGINE_NAME}
        PRIVATE cpp_redis::cpp_redis cxxopts::cxxopts spdlog::spdlog prometheus-cpp::core prometheus-cpp::pull
        PUBLIC ${${PROJECT_PREFIX}_ROUTER_PROTO_BINDING_NAME})
target_include_directories(${${PROJECT_PREFIX}_ROUTER_ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${${PROJECT_PREFIX}_ROUTER_APP_NAME} PRIVATE ${${PROJECT_PREFIX}_ROUTER_ENGINE_NAME})

install(TARGETS ${${PROJECT_PREFIX}_ROUTER_APP_NAME}
		RUNTIME DESTINATION ${${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX}/bin
		COMPONENT ${${PROJECT_PREFIX}_ROUTER_COMPONENT_NAME}
		)

if(${PROJECT_PREFIX}_PACKAGE_BUILD_RPM)
	cpack_add_component(${${PROJECT_PREFIX}_ROUTER_COMPONENT_NAME})
endif()

if (${PROJECT_PREFIX}_AUTO_INSTALL_RUNTIME_DEPS)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake
            CONTENT "
    		file(GET_RUNTIME_DEPENDENCIES
    			RESOLVED_DEPENDENCIES_VAR resolved_deps
    			UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
    			EXECUTABLES $<TARGET_FILE:${${PROJECT_PREFIX}_ROUTER_APP_NAME}>)
    
    			file(INSTALL FOLLOW_SYMLINK_CHAIN DESTINATION \"${CMAKE_INSTALL_PREFIX}/${${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX}/bin\" TYPE EXECUTABLE FILES \${resolved_deps})
    	")
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake)
endif ()

add_subdirectory(tests)