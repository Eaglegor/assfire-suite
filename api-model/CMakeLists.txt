find_package(protobuf CONFIG REQUIRED)

add_library(assfire-api-model)
protobuf_generate(
        LANGUAGE cpp
        TARGET assfire-api-model
        PROTOS
        assfire/api/v1/optimization/vrp/vrp.proto
        assfire/api/v1/routing/routing.proto
        assfire/api/v1/sample/sample.proto
        IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(assfire-api-model PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(assfire-api-model PUBLIC protobuf::libprotobuf)