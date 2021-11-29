if (ASSFIRE_GRPC_GATEWAYS_ENABLED)

    define_go_grpc_proxy_target(
            TARGET_NAME assfire-tsp-grpc-gateway
            DEPENDS assfire-tsp-proto-go
            PROTOS
            assfire/api/v1/tsp/service.proto
            GO_PACKAGE_PREFIX assfire.org/api/v1
            GO_SUBPACKAGE_NAME assfire.org/gateway/tsp
            RPM_COMPONENT_NAME tsp-grpc-gateway
    )

endif ()

