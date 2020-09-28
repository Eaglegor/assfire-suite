find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_library(assfire-router-proto-binding)

protobuf_generate(
        LANGUAGE cpp
        TARGET assfire-router-proto-binding
        PROTOS assfire/routing/v1/routing.proto
)

get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(
        LANGUAGE grpc
        PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
        PROTOS assfire/routing/v1/routing.proto
        TARGET assfire-router-proto-binding
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

set_target_properties(assfire-router-proto-binding PROPERTIES
        PUBLIC_HEADERS "routing.pb.h;routing.grpc.pb.h")
target_include_directories(assfire-router-proto-binding PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(assfire-router-proto-binding PUBLIC gRPC::grpc++ protobuf::libprotobuf)

if (ASSFIRE_GENERATE_GRPC_REVERSE_PROXIES)
    include(reverse_proxy)
    GenerateGrpcReverseProxy(
            OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/go
            TARGET_NAME assfire-router-reverse-proxy
            SERVICE_DEFINITION assfire/routing/v1/routing.yaml
            PROTO_DEFINITION assfire/routing/v1/routing.proto
            GO_ENTRY_POINT assfire/routing/v1/routing.go
            GO_MOD assfire/routing/v1/go.mod
            GO_MODULE_ROOT go/assfire/routing
            INSTALL_PREFIX ${ASSFIRE_ROUTER_INSTALL_PREFIX}/bin
    )
endif ()