include(proto_utils)

set(PROTO_PREFIX ${CMAKE_SOURCE_DIR}/apps/router/proto)

define_proto_go_target(
        TARGET_NAME assfire-router-proto-go
        PROTO_IMPORT_DIRS ${PROTO_PREFIX}
        DEPENDS assfire-concepts-proto-go
        PROTOS assfire/api/v1/router/concepts.proto
        PROTO_ROOT ${PROTO_PREFIX}
        GO_PACKAGE_NAME assfire.org/api/v1/router
)