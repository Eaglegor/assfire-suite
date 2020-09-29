find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_library(${${PROJECT_PREFIX}_ROUTER_PROTO_BINDING_NAME})

protobuf_generate(
        LANGUAGE cpp
        TARGET ${${PROJECT_PREFIX}_ROUTER_PROTO_BINDING_NAME}
        PROTOS assfire/routing/v1/routing.proto
)

get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(
        LANGUAGE grpc
        PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
        PROTOS assfire/routing/v1/routing.proto
        TARGET ${${PROJECT_PREFIX}_ROUTER_PROTO_BINDING_NAME}
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

set_target_properties(${${PROJECT_PREFIX}_ROUTER_PROTO_BINDING_NAME} PROPERTIES
        PUBLIC_HEADERS "routing.pb.h;routing.grpc.pb.h")
target_include_directories(${${PROJECT_PREFIX}_ROUTER_PROTO_BINDING_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${${PROJECT_PREFIX}_ROUTER_PROTO_BINDING_NAME} PUBLIC gRPC::grpc++ protobuf::libprotobuf)

if (${PROJECT_PREFIX}_GENERATE_GRPC_REVERSE_PROXIES)
    include(reverse_proxy)
    GenerateGrpcReverseProxy(
            OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/go
            TARGET_NAME ${${PROJECT_PREFIX}_ROUTER_REVERSE_PROXY_NAME}
            COMPONENT_NAME ${${PROJECT_PREFIX}_ROUTER_SHORT_REVERSE_PROXY_NAME}
            SERVICE_DEFINITION assfire/routing/v1/routing.yaml
            PROTO_DEFINITION assfire/routing/v1/routing.proto
            GO_ENTRY_POINT assfire/routing/v1/routing.go
            GO_MOD assfire/routing/v1/go.mod
            GO_MODULE_ROOT go/assfire/routing
            INSTALL_PREFIX ${${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX}/bin
    )

endif ()