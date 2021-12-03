include(proto_utils)

set(PROTO_PREFIX ${CMAKE_SOURCE_DIR}/apps/scheduler/proto)

define_proto_go_target(
        TARGET_NAME assfire-scheduler-proto-go
        PROTO_IMPORT_DIRS ${PROTO_PREFIX}
        DEPENDS
        assfire-concepts-proto-go
        assfire-router-proto-go
        assfire-locations-proto-go
        PROTOS assfire/api/v1/scheduler/concepts.proto
        GRPC_PROTOS assfire/api/v1/scheduler/service.proto
        PROTO_ROOT ${PROTO_PREFIX}
        GO_PACKAGE_NAME assfire.org/api/v1/scheduler
        REQUIRES
        "google.golang.org/genproto v0.0.0-20211118181313-81c1377c94b1"
        "google.golang.org/grpc v1.42.0"
)