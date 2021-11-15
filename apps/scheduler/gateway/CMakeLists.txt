
if (ASSFIRE_GENERATE_GRPC_GATEWAYS)

#    define_go_grpc_proxy_target(
#            TARGET_NAME assfire-scheduler-grpc-gateway
#            DEPENDS assfire-scheduler-grpc
#            PROTOS
#            assfire/api/v1/concepts/concepts.proto
#            assfire/api/v1/router/concepts.proto
#            assfire/api/v1/scheduler/concepts.proto
#            assfire/api/v1/scheduler/service.proto
#            GW_MODULE assfire.org/api/v1/scheduler
#            RPM_COMPONENT_NAME scheduler-grpc-gateway
#    )

endif ()

