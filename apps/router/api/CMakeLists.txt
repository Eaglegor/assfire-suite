find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)

set(IMPORT_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/api-model
        ${GOOGLE_PROTOBUF_IMPORT_DIR})
set(PROTOS
        assfire/api/v1/service/router/service.proto
        )

add_library(assfire-router-api)

protobuf_generate(TARGET assfire-router-api
        LANGUAGE cpp
        PROTOS ${PROTOS}
        IMPORT_DIRS ${IMPORT_DIRS}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/api/cpp)

protobuf_generate(TARGET assfire-router-api
        LANGUAGE grpc
        PROTOS ${PROTOS}
        IMPORT_DIRS ${IMPORT_DIRS}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/api/cpp
        PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc)

target_include_directories(assfire-router-api PUBLIC ${CMAKE_BINARY_DIR}/api/cpp)
target_link_libraries(assfire-router-api PUBLIC gRPC::grpc++ protobuf::libprotobuf assfire-api-model)