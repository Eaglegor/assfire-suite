find_package(protobuf CONFIG REQUIRED)

add_library(assfire-api-model)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/api)
protobuf_generate(
        LANGUAGE cpp
        TARGET assfire-api-model

        PROTOS
        assfire/api/v1/model/optimization/vrp/vrp.proto
        assfire/api/v1/model/routing/routing.proto
        assfire/api/v1/model/sample/sample.proto

        IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/api
)
target_include_directories(assfire-api-model PUBLIC ${CMAKE_BINARY_DIR}/api)
target_link_libraries(assfire-api-model PUBLIC protobuf::libprotobuf)