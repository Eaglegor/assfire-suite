
if (ASSFIRE_GENERATE_GRPC_GATEWAYS)

    define_go_grpc_proxy_target(
            TARGET_NAME assfire-router-grpc-gateway
            DEPENDS assfire-router-grpc assfire-router-proto-go
            PROTOS
            assfire/api/v1/router/service.proto
            GO_PACKAGE_NAME assfire.org/api/v1/router
            RPM_COMPONENT_NAME router-grpc-gateway
    )

endif ()

