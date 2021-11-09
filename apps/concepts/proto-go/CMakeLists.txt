include(proto_utils)

set(PROTO_PREFIX ${CMAKE_SOURCE_DIR}/apps/concepts/proto)

define_proto_go_target(
        TARGET_NAME assfire-concepts-proto-go
        PROTO_IMPORT_DIRS ${PROTO_PREFIX}
        DEPENDS ""
        PROTOS assfire/api/v1/concepts/concepts.proto
        PROTO_ROOT ${PROTO_PREFIX}
        GO_PACKAGE_NAME assfire/api/v1/concepts
)