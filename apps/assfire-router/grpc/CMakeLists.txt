find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_library(routes_provider_protobuf_binding)

protobuf_generate(
        LANGUAGE cpp
        TARGET routes_provider_protobuf_binding
        PROTOS assfire/routing/v1/routing.proto
)

get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(
        LANGUAGE grpc
        PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
        PROTOS assfire/routing/v1/routing.proto
        TARGET routes_provider_protobuf_binding
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

set_target_properties(routes_provider_protobuf_binding PROPERTIES
        PUBLIC_HEADERS "routing.pb.h;routing.grpc.pb.h")
target_include_directories(routes_provider_protobuf_binding PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(routes_provider_protobuf_binding PUBLIC gRPC::grpc++ protobuf::libprotobuf)

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
            INSTALL_PREFIX ${ROUTES_PROVIDER_SERVER_INSTALL_PREFIX}
    )
endif ()