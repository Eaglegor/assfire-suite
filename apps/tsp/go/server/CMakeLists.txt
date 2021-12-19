include(go_utils)

set(SOURCES
        amqp_connector.go
        interrupt_signal_publisher.go
        main.go
        redis_connector.go
        solution_storage.go
        status_enforcer.go
        status_provider.go
        task_publisher.go
        task_queue.go
        task_storage.go
        tsp_server.go
        tsp_server_factory.go
        util.go
        worker_status_listener.go
        )

if (ASSFIRE_IN_SOURCE_GO_ENABLED)
    set(ASSFIRE_TSP_SERVER_GO_MOD_PATH ${CMAKE_CURRENT_SOURCE_DIR})
else ()
    set(ASSFIRE_TSP_SERVER_GO_MOD_PATH ${CMAKE_CURRENT_BINARY_DIR})

    foreach (file ${SOURCES})
        configure_file(${file} ${file})
    endforeach ()

endif ()

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
        "github.com/golang/protobuf v1.5.2"
        "github.com/sirupsen/logrus v1.8.1"
        RPM_COMPONENT_NAME tsp-server
        GO_MOD_PATH ${ASSFIRE_TSP_SERVER_GO_MOD_PATH}
)