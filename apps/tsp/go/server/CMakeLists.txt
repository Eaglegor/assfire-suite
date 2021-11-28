include(proto_utils)

if(NOT ASSFIRE_IN_SOURCE_GO_ENABLED)
    set(ASSFIRE_TSP_SERVER_GO_MOD_PATH ${CMAKE_CURRENT_BINARY_DIR})
    configure_file(server.go server.go)
endif()

define_go_executable_target(
        TARGET_NAME assfire-tsp-server
        DEPENDS assfire-tsp-proto-go
        GO_PACKAGE_NAME assfire.org/tsp/server
        REQUIRES
        "github.com/google/uuid v1.3.0"
        "github.com/go-redis/redis/v8 v8.11.4"
        "github.com/streadway/amqp v1.0.0"
        "github.com/grpc-ecosystem/grpc-gateway/v2 v2.6.0"
        "google.golang.org/grpc v1.42.0"
        RPM_COMPONENT_NAME tsp-server
        GO_MOD_PATH ${ASSFIRE_TSP_SERVER_GO_MOD_PATH}
)