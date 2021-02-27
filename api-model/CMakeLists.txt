add_library(assfire-api-model)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/proto/cpp)
protobuf_generate(
        LANGUAGE cpp
        TARGET assfire-api-model

        PROTOS
        assfire/api/v1/model/optimization/transport/transport.proto
        assfire/api/v1/model/routing/routing.proto
        assfire/api/v1/model/sample/sample.proto

        ../apps/google-annotations/google/api/annotations.proto
        ../apps/google-annotations/google/api/http.proto

        IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR};${GOOGLE_PROTOBUF_IMPORT_DIR}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/proto/cpp
)
target_include_directories(assfire-api-model PUBLIC ${CMAKE_BINARY_DIR}/proto/cpp)
target_link_libraries(assfire-api-model PUBLIC protobuf::libprotobuf)

set(GOOGLE_PROTOBUF_IMPORT_DIR ${GOOGLE_PROTOBUF_IMPORT_DIR} PARENT_SCOPE)