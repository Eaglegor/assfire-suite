
if (ASSFIRE_GENERATE_GRPC_GATEWAYS)

    define_go_grpc_proxy_target(
            TARGET_NAME assfire-router-grpc-gateway
            DEPENDS assfire-router-proto-go
            PROTOS
            assfire/api/v1/router/service.proto
            GO_PACKAGE_PREFIX assfire.org/api/v1
            GO_SUBPACKAGE_NAME assfire.org/gateway/router
            RPM_COMPONENT_NAME router-grpc-gateway
    )

endif ()

