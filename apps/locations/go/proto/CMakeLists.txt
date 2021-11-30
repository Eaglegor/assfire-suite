include (proto_utils)

set(PROTO_PREFIX ${CMAKE_SOURCE_DIR}/apps/locations/proto)

define_proto_go_target(
        TARGET_NAME assfire-locations-proto-go
        PROTO_IMPORT_DIRS ${PROTO_PREFIX}
        DEPENDS assfire-concepts-proto-go
        GRPC_PROTOS assfire/api/v1/locations/service.proto
        PROTO_ROOT ${PROTO_PREFIX}
        GO_PACKAGE_NAME assfire.org/api/v1/locations
)