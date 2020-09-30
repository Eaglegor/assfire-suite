list(APPEND ASSFIRE_ROUTER_CLIENT_PUBLIC_HEADERS
		assfire-router/client.hpp
		assfire-router/DistanceMatrix.hpp
		assfire-router/DistanceMatrixStrategy.hpp
		assfire-router/Location.hpp
		assfire-router/RouteInfo.hpp
		assfire-router/RoutingClient.hpp
		assfire-router/strategies/DirectRequestStrategy.hpp
		assfire-router/grpc_client/GrpcClient.hpp
		assfire-router/settings/EuclideanSettings.hpp
		assfire-router/settings/RandomSettings.hpp
		assfire-router/settings/RouteRequestSettings.hpp
		assfire-router/settings/FixedPointIntCoordinatesFormat.hpp
)

add_library(${ASSFIRE_ROUTER_CLIENT_NAME}
    "assfire-router/strategies/DirectRequestStrategy.cpp"
    "assfire-router/grpc_client/GrpcClient.cpp"
    ${ASSFIRE_ROUTER_CLIENT_PUBLIC_HEADERS}
)

set_target_properties(${ASSFIRE_ROUTER_CLIENT_NAME} PROPERTIES PUBLIC_HEADER "${ASSFIRE_ROUTER_CLIENT_PUBLIC_HEADERS}")

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${ASSFIRE_ROUTER_CLIENT_NAME} PRIVATE ${ASSFIRE_ROUTER_PROTO_BINDING_NAME} spdlog::spdlog)
target_include_directories(${ASSFIRE_ROUTER_CLIENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${ASSFIRE_ROUTER_CLIENT_NAME}
        RUNTIME DESTINATION ${ASSFIRE_ROUTER_INSTALL_PREFIX}/bin
		ARCHIVE DESTINATION ${ASSFIRE_ROUTER_INSTALL_PREFIX}/lib
		COMPONENT ${ASSFIRE_ROUTER_SHORT_CLIENT_NAME}
        PUBLIC_HEADER DESTINATION ${ASSFIRE_ROUTER_INSTALL_PREFIX}/include)

if(ASSFIRE_PACKAGE_BUILD_RPM)
	cpack_add_component(${ASSFIRE_ROUTER_SHORT_CLIENT_NAME})
endif()

if(ASSFIRE_AUTO_INSTALL_RUNTIME_DEPS)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake
    	CONTENT "
    		file(GET_RUNTIME_DEPENDENCIES
    			RESOLVED_DEPENDENCIES_VAR resolved_deps
    			UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
    			EXECUTABLES $<TARGET_FILE:assfire_router_client>)
    
    			file(INSTALL FOLLOW_SYMLINK_CHAIN DESTINATION \"${ASSFIRE_ROUTER_INSTALL_PREFIX}/bin\" TYPE EXECUTABLE FILES \${resolved_deps})
    	")
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake)
endif()



add_subdirectory(tests)
