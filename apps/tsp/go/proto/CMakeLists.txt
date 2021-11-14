include(proto_utils)

set(PROTO_PREFIX ${CMAKE_SOURCE_DIR}/apps/tsp/proto)

define_proto_go_target(
        TARGET_NAME assfire-tsp-proto-go
        PROTO_IMPORT_DIRS ${PROTO_PREFIX}
        DEPENDS
        assfire-concepts-proto-go
        assfire-router-proto-go
        PROTOS
        assfire/api/v1/tsp/concepts.proto
        assfire/api/v1/tsp/state.proto
        assfire/api/v1/tsp/worker.proto
        GRPC_PROTOS
        assfire/api/v1/tsp/service.proto
        PROTO_ROOT ${PROTO_PREFIX}
        GO_PACKAGE_NAME assfire.org/api/v1/tsp
)