list(APPEND ${PROJECT_PREFIX}_ROUTER_CLIENT_PUBLIC_HEADERS
		assfire-router/client.hpp
		assfire-router/DistanceMatrix.hpp
		assfire-router/DistanceMatrixBackend.hpp
		assfire-router/Location.hpp
		assfire-router/RouteInfo.hpp
		assfire-router/RoutingClient.hpp
		assfire-router/backends/DirectRequestBackend.hpp
		assfire-router/grpc_client/GrpcClient.hpp
		assfire-router/settings/EuclideanSettings.hpp
		assfire-router/settings/RandomSettings.hpp
		assfire-router/settings/RouteRequestSettings.hpp
		assfire-router/settings/FixedPointIntCoordinatesFormat.hpp
)

add_library(${${PROJECT_PREFIX}_ROUTER_CLIENT_NAME}
    "assfire-router/backends/DirectRequestBackend.cpp"
    "assfire-router/grpc_client/GrpcClient.cpp"
    ${${PROJECT_PREFIX}_ROUTER_CLIENT_PUBLIC_HEADERS}
)

set_target_properties(${${PROJECT_PREFIX}_ROUTER_CLIENT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_PREFIX}_ROUTER_CLIENT_PUBLIC_HEADERS}")

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${${PROJECT_PREFIX}_ROUTER_CLIENT_NAME} PRIVATE ${${PROJECT_PREFIX}_ROUTER_PROTO_BINDING_NAME} spdlog::spdlog)
target_include_directories(${${PROJECT_PREFIX}_ROUTER_CLIENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${${PROJECT_PREFIX}_ROUTER_CLIENT_NAME}
        RUNTIME DESTINATION ${${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX}/bin
		ARCHIVE DESTINATION ${${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX}/lib
		COMPONENT ${${PROJECT_PREFIX}_ROUTER_SHORT_CLIENT_NAME}
        PUBLIC_HEADER DESTINATION ${${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX}/include)

if(${PROJECT_PREFIX}_PACKAGE_BUILD_RPM)
	cpack_add_component(${${PROJECT_PREFIX}_ROUTER_SHORT_CLIENT_NAME}
			DISPLAY_NAME "Assfire Router Client")
endif()

if(${PROJECT_PREFIX}_AUTO_INSTALL_RUNTIME_DEPS)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake
    	CONTENT "
    		file(GET_RUNTIME_DEPENDENCIES
    			RESOLVED_DEPENDENCIES_VAR resolved_deps
    			UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
    			EXECUTABLES $<TARGET_FILE:assfire_router_client>)
    
    			file(INSTALL FOLLOW_SYMLINK_CHAIN DESTINATION \"${${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX}/bin\" TYPE EXECUTABLE FILES \${resolved_deps})
    	")
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake)
endif()



add_subdirectory(tests)
