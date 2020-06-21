add_executable(routes_provider_server 
		main.cpp 
		backends/euclidean/EuclideanRouter.cpp
 "RoutingService.hpp" "RoutingService.cpp")

find_package(protobuf CONFIG REQUIRED)
protobuf_generate(
	LANGUAGE cpp
	TARGET routes_provider_server
	PROTOS routing.proto
)

find_package(gRPC CONFIG REQUIRED)

get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)
message(STATUS ${gRPC_CPP_PLUGIN_EXECUTABLE})
protobuf_generate(
	LANGUAGE grpc
	PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
	PROTOS routing.proto
	TARGET routes_provider_server
	GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

target_link_libraries(routes_provider_server gRPC::grpc++)