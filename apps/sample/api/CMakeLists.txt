set(TARGET_NAME assfire-sample-api)
set(PROTOS assfire/api/v1/service/sample/service.proto)
set(IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/api-model)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)

add_library(${TARGET_NAME})

protobuf_generate(TARGET ${TARGET_NAME}
        LANGUAGE cpp
        PROTOS ${PROTOS}
        IMPORT_DIRS ${IMPORT_DIRS}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/api)

protobuf_generate(TARGET ${TARGET_NAME}
        LANGUAGE grpc
        PROTOS ${PROTOS}
        IMPORT_DIRS ${IMPORT_DIRS}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/api
        PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc)

target_link_libraries(${TARGET_NAME} PUBLIC gRPC::grpc++ protobuf::libprotobuf assfire-api-model)
