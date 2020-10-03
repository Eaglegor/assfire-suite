find_package(protobuf CONFIG REQUIRED)
get_target_property(GOOGLE_PROTOBUF_IMPORT_DIR protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

add_library(assfire-api-model ../utils/assfire/win32/winsock_initializer.h)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/api/cpp)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/api/go)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/api/swagger)
protobuf_generate(
        LANGUAGE cpp
        TARGET assfire-api-model

        PROTOS
        assfire/api/v1/model/optimization/vrp/vrp.proto
        assfire/api/v1/model/routing/routing.proto
        assfire/api/v1/model/sample/sample.proto

        google/api/annotations.proto
        google/api/http.proto

        IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR};${GOOGLE_PROTOBUF_IMPORT_DIR}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/api/cpp
)
target_include_directories(assfire-api-model PUBLIC ${CMAKE_BINARY_DIR}/api/cpp)
target_link_libraries(assfire-api-model PUBLIC protobuf::libprotobuf)

set(GOOGLE_PROTOBUF_IMPORT_DIR ${GOOGLE_PROTOBUF_IMPORT_DIR} PARENT_SCOPE)