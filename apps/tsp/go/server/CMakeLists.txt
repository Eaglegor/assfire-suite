include(proto_utils)

define_go_executable_target(
        TARGET_NAME assfire-tsp-server
        DEPENDS assfire-tsp-proto-go
        GO_PACKAGE_NAME assfire.org/tsp/server
        REQUIRES
        "github.com/google/uuid v1.3.0"
        "github.com/go-redis/redis/v8 v8.11.4"
        "github.com/streadway/amqp v1.0.0"
        "github.com/grpc-ecosystem/grpc-gateway/v2 v2.6.0"
        "google.golang.org/grpc v1.35.0"
        RPM_COMPONENT_NAME tsp-server
)