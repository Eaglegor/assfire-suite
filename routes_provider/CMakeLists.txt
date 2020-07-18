add_executable(assfire-router 
		main.cpp 
		)

add_library(routes_provider_server_engine
		RoutingService.cpp
		backends/euclidean/EuclideanRouter.cpp
		backends/random/RandomRouter.cpp
		backends/redis/RedisProxy.cpp
		backends/redis/RouterProxy.hpp 
		backends/redis/TransparentProxy.hpp
		"RoutingMetricsCollector.cpp"
		"RoutingMetricsCollector.hpp"
)

add_library(routes_provider_protobuf_binding)

find_package(protobuf CONFIG REQUIRED)
protobuf_generate(
	LANGUAGE cpp
	TARGET routes_provider_protobuf_binding
	PROTOS routing.proto
)

find_package(gRPC CONFIG REQUIRED)

get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(
	LANGUAGE grpc
	PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
	PROTOS routing.proto
	TARGET routes_provider_protobuf_binding
	GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

set_target_properties(routes_provider_protobuf_binding PROPERTIES
	PUBLIC_HEADERS "routing.pb.h;routing.grpc.pb.h")
target_include_directories(routes_provider_protobuf_binding PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

find_package(cxxopts CONFIG REQUIRED)

find_package(spdlog CONFIG REQUIRED)

include(pkg_utils)
vcpkg_find_cpp_redis()

target_link_libraries(routes_provider_protobuf_binding PUBLIC gRPC::grpc++ protobuf::libprotobuf)

find_package(civetweb CONFIG REQUIRED) # Required by prometheus 
find_package(prometheus-cpp CONFIG REQUIRED)

target_link_libraries(routes_provider_server_engine 
	PRIVATE cpp_redis::cpp_redis cxxopts::cxxopts spdlog::spdlog prometheus-cpp::core prometheus-cpp::pull
	PUBLIC routes_provider_protobuf_binding)
target_include_directories(routes_provider_server_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(assfire-router PRIVATE routes_provider_server_engine)

if(ASSFIRE_BUNDLE_INSTALL_MODE)
	set(ROUTES_PROVIDER_SERVER_BUNDLE_PREFIX "assfire-router")
endif()

set(ROUTES_PROVIDER_SERVER_INSTALL_PREFIX ${ROUTES_PROVIDER_SERVER_BUNDLE_PREFIX}/bin)
install(TARGETS assfire-router RUNTIME DESTINATION ${ROUTES_PROVIDER_SERVER_INSTALL_PREFIX})

if(ASSFIRE_AUTO_INSTALL_RUNTIME_DEPS)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake
    	CONTENT "
    		file(GET_RUNTIME_DEPENDENCIES
    			RESOLVED_DEPENDENCIES_VAR resolved_deps
    			UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
    			EXECUTABLES $<TARGET_FILE:assfire-router>)
    
    			file(INSTALL FOLLOW_SYMLINK_CHAIN DESTINATION \"${CMAKE_INSTALL_PREFIX}/${ROUTES_PROVIDER_SERVER_INSTALL_PREFIX}\" TYPE EXECUTABLE FILES \${resolved_deps})
    	")
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/install_deps.cmake)
endif()

add_subdirectory(client)
add_subdirectory(tests)