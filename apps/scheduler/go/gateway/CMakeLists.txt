
if (ASSFIRE_GRPC_GATEWAYS_ENABLED)

    define_go_grpc_proxy_target(
            TARGET_NAME assfire-scheduler-grpc-gateway
            DEPENDS assfire-scheduler-proto-go
            PROTOS
            assfire/api/v1/scheduler/service.proto
            GO_PACKAGE_PREFIX assfire.org/api/v1
            GO_SUBPACKAGE_NAME assfire.org/gateway/scheduler
            RPM_COMPONENT_NAME scheduler-grpc-gateway
    )

endif ()

