include(go_utils)

if(ASSFIRE_IN_SOURCE_GO_ENABLED)
    set(ASSFIRE_LOCATIONS_SERVER_GO_MOD_PATH ${CMAKE_CURRENT_SOURCE_DIR})
else()
    set(ASSFIRE_LOCATIONS_SERVER_GO_MOD_PATH ${CMAKE_CURRENT_BINARY_DIR})
    configure_file(server.go server.go)
endif()

define_go_executable_target(
        TARGET_NAME assfire-locations-server
        DEPENDS assfire-locations-proto-go
        GO_PACKAGE_NAME assfire.org/locations/server
        REQUIRES
        "github.com/go-redis/redis/v8 v8.11.4"
        "github.com/streadway/amqp v1.0.0"
        "github.com/grpc-ecosystem/grpc-gateway/v2 v2.6.0"
        "google.golang.org/grpc v1.42.0"
        "go.mongodb.org/mongo-driver v1.8.0"
        RPM_COMPONENT_NAME locations-server
        GO_MOD_PATH ${ASSFIRE_LOCATIONS_SERVER_GO_MOD_PATH}
)