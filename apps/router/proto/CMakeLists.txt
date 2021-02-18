find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)

set(SOURCES
        assfire/api/v1/service/router/translators/LocationTranslator.hpp assfire/api/v1/service/router/translators/RouteInfoTranslator.hpp)
set(HEADERS
        assfire/api/v1/service/router/translators/RoutingEngineTypeTranslator.hpp
        assfire/api/v1/service/router/translators/RoutingProfileTranslator.hpp
        assfire/api/v1/service/router/translators/RoutingOptionsTranslator.hpp
        )

set(IMPORT_DIRS
        ../proto
        ${CMAKE_SOURCE_DIR}/api-model
        ${GOOGLE_PROTOBUF_IMPORT_DIR})
set(PROTOS
        assfire/api/v1/service/router/router.proto
        )

add_library(assfire-router-proto ${SOURCES})

protobuf_generate(TARGET assfire-router-proto
        LANGUAGE cpp
        PROTOS ${PROTOS}
        IMPORT_DIRS ${IMPORT_DIRS}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/proto/cpp)

protobuf_generate(TARGET assfire-router-proto
        LANGUAGE grpc
        PROTOS ${PROTOS}
        IMPORT_DIRS ${IMPORT_DIRS}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/proto/cpp
        PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc)

target_include_directories(assfire-router-proto PUBLIC ${CMAKE_BINARY_DIR}/proto/cpp ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(assfire-router-proto PUBLIC gRPC::grpc++ protobuf::libprotobuf assfire-api-model assfire-router-api)
