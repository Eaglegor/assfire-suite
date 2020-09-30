add_executable(${ASSFIRE_ROUTER_APP_NAME} main.cpp)

add_library(${ASSFIRE_ROUTER_ENGINE_NAME}
		RoutingService.cpp
		backends/euclidean/EuclideanRouter.cpp
		backends/random/RandomRouter.cpp
		backends/crowflight/CrowflightRouter.cpp
		backends/redis/RedisProxy.cpp
		backends/redis/RouterProxy.hpp
		backends/redis/TransparentProxy.hpp
        RoutingMetricsCollector.cpp
        RoutingMetricsCollector.hpp
		backends/osrm/HttpOsrmRouter.cpp backends/osrm/HttpOsrmRouter.hpp)

find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(cpprestsdk REQUIRED)

include(pkg_utils)
vcpkg_find_cpp_redis()

find_package(civetweb CONFIG REQUIRED) # Required by prometheus
find_package(prometheus-cpp CONFIG REQUIRED)

target_link_libraries(${ASSFIRE_ROUTER_ENGINE_NAME}
        PRIVATE cpp_redis::cpp_redis cxxopts::cxxopts spdlog::spdlog prometheus-cpp::core prometheus-cpp::pull cpprestsdk::cpprest
        PUBLIC ${ASSFIRE_ROUTER_PROTO_BINDING_NAME})
target_include_directories(${ASSFIRE_ROUTER_ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${ASSFIRE_ROUTER_APP_NAME} PRIVATE ${ASSFIRE_ROUTER_ENGINE_NAME})

install(TARGETS ${ASSFIRE_ROUTER_APP_NAME}
		RUNTIME DESTINATION ${ASSFIRE_ROUTER_INSTALL_PREFIX}/bin
		COMPONENT ${ASSFIRE_ROUTER_COMPONENT_NAME}
		)

if(ASSFIRE_PACKAGE_BUILD_RPM)
	cpack_add_component(${ASSFIRE_ROUTER_COMPONENT_NAME})
endif()

if (ASSFIRE_AUTO_INSTALL_RUNTIME_DEPS)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake
            CONTENT "
    		file(GET_RUNTIME_DEPENDENCIES
    			RESOLVED_DEPENDENCIES_VAR resolved_deps
    			UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
    			EXECUTABLES $<TARGET_FILE:${ASSFIRE_ROUTER_APP_NAME}>)
    
    			file(INSTALL FOLLOW_SYMLINK_CHAIN DESTINATION \"${CMAKE_INSTALL_PREFIX}/${ASSFIRE_ROUTER_INSTALL_PREFIX}/bin\" TYPE EXECUTABLE FILES \${resolved_deps})
    	")
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake)
endif ()

add_subdirectory(tests)