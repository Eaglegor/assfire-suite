include(proto_utils)

define_proto_target(
        TARGET_NAME assfire-google-annotations
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ""
        PROTOS google/api/annotations.proto google/api/http.proto
)
