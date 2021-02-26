find_package(protobuf CONFIG REQUIRED)
get_target_property(GOOGLE_PROTOBUF_IMPORT_DIR protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

add_library(assfire-api-model ../apps/scheduler/api/assfire/scheduler/api/model/WaybillSchedulable.hpp ../libs/route_optimization_concepts/assfire/concepts/TimePoint.hpp ../libs/route_optimization_concepts/assfire/concepts/TimePoint.cpp ../libs/route_optimization_concepts/assfire/concepts/TimeInterval.cpp)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/proto/cpp)
protobuf_generate(
        LANGUAGE cpp
        TARGET assfire-api-model

        PROTOS
        assfire/api/v1/model/optimization/transport/transport.proto
        assfire/api/v1/model/routing/routing.proto
        assfire/api/v1/model/sample/sample.proto

        google/api/annotations.proto
        google/api/http.proto

        IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR};${GOOGLE_PROTOBUF_IMPORT_DIR}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/proto/cpp
)
target_include_directories(assfire-api-model PUBLIC ${CMAKE_BINARY_DIR}/proto/cpp)
target_link_libraries(assfire-api-model PUBLIC protobuf::libprotobuf)

set(GOOGLE_PROTOBUF_IMPORT_DIR ${GOOGLE_PROTOBUF_IMPORT_DIR} PARENT_SCOPE)