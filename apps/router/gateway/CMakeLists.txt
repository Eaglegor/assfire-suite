
if (ASSFIRE_GENERATE_GRPC_GATEWAYS)

    define_go_grpc_proxy_target(
            TARGET_NAME assfire-router-grpc-gateway
            DEPENDS assfire-router-grpc
            PROTOS
            assfire/api/v1/concepts/concepts.proto
            assfire/api/v1/router/concepts.proto
            assfire/api/v1/router/service.proto
            GW_MODULE assfire.org/api/v1/router
            RPM_COMPONENT_NAME router-grpc-gateway
    )

endif ()

