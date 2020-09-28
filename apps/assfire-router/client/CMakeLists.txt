list(APPEND ASSFIRE_ROUTER_CLIENT_PUBLIC_HEADERS
    assfire_router/client.hpp
    assfire_router/DistanceMatrix.hpp
    assfire_router/DistanceMatrixBackend.hpp
    assfire_router/Location.hpp
    assfire_router/RouteInfo.hpp
    assfire_router/RoutingClient.hpp
    assfire_router/backends/DirectRequestBackend.hpp
    assfire_router/grpc_client/GrpcClient.hpp
    assfire_router/settings/EuclideanSettings.hpp
    assfire_router/settings/RandomSettings.hpp
    assfire_router/settings/RouteRequestSettings.hpp
    assfire_router/settings/FixedPointIntCoordinatesFormat.hpp
)

add_library(assfire_router_client
    "assfire_router/backends/DirectRequestBackend.cpp"
    "assfire_router/grpc_client/GrpcClient.cpp"
    ${ASSFIRE_ROUTER_CLIENT_PUBLIC_HEADERS}
)

set_target_properties(assfire_router_client PROPERTIES PUBLIC_HEADER "${ASSFIRE_ROUTER_CLIENT_PUBLIC_HEADERS}")

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(assfire_router_client PRIVATE routes_provider_protobuf_binding spdlog::spdlog)
target_include_directories(assfire_router_client INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(ASSFIRE_BUNDLE_INSTALL_MODE)
	set(ROUTES_PROVIDER_CLIENT_BUNDLE_PREFIX "assfire_router_client")
endif()

set(ROUTES_PROVIDER_CLIENT_INSTALL_PREFIX ${ROUTES_PROVIDER_CLIENT_BUNDLE_PREFIX}/bin)
install(TARGETS assfire_router_client 
        RUNTIME DESTINATION ${ROUTES_PROVIDER_CLIENT_INSTALL_PREFIX}
        PUBLIC_HEADER DESTINATION ${ROUTES_PROVIDER_CLIENT_BUNDLE_PREFIX}/include)
        
if(ASSFIRE_AUTO_INSTALL_RUNTIME_DEPS)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake
    	CONTENT "
    		file(GET_RUNTIME_DEPENDENCIES
    			RESOLVED_DEPENDENCIES_VAR resolved_deps
    			UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
    			EXECUTABLES $<TARGET_FILE:assfire_router_client>)
    
    			file(INSTALL FOLLOW_SYMLINK_CHAIN DESTINATION \"${CMAKE_INSTALL_PREFIX}/${ROUTES_PROVIDER_CLIENT_INSTALL_PREFIX}\" TYPE EXECUTABLE FILES \${resolved_deps})
    	")
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake)
endif()

add_subdirectory(tests)

