include(proto_utils)

define_proto_target(
        TARGET_NAME assfire-concepts-proto
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ""
        PROTOS assfire/api/v1/concepts/concepts.proto
)
