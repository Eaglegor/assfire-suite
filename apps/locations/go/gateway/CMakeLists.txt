if (ASSFIRE_GRPC_GATEWAYS_ENABLED)

    define_go_grpc_proxy_target(
            TARGET_NAME assfire-locations-grpc-gateway
            DEPENDS assfire-locations-proto-go
            PROTOS
            assfire/api/v1/locations/service.proto
            GO_PACKAGE_PREFIX assfire.org/api/v1
            GO_SUBPACKAGE_NAME assfire.org/gateway/locations
            RPM_COMPONENT_NAME locations-grpc-gateway
    )
endif ()
