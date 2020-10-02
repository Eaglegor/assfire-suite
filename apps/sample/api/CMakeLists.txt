find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)

add_library(${ASSFIRE_SAMPLE_API_NAME})

protobuf_generate(
        LANGUAGE cpp
        TARGET ${ASSFIRE_SAMPLE_API_NAME}
        PROTOS ${ASSFIRE_SAMPLE_APP_SOURCE_ROOT}/api/service.proto
        IMPORT_DIRS ${CMAKE_SOURCE_DIR}/api
)

protobuf_generate(
        LANGUAGE grpc
        PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
        PROTOS ${ASSFIRE_SAMPLE_APP_SOURCE_ROOT}/api/service.proto
        TARGET ${ASSFIRE_SAMPLE_API_NAME}
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

set_target_properties(${ASSFIRE_SAMPLE_API_NAME} PROPERTIES
        PUBLIC_HEADERS "sample.pb.h;sample.grpc.pb.h")
target_include_directories(${ASSFIRE_SAMPLE_API_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${ASSFIRE_SAMPLE_API_NAME} PUBLIC gRPC::grpc++ protobuf::libprotobuf assfire-api-model)
