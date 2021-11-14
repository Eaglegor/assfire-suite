if(ASSFIRE_APPLICATION_RELEASE_VERSION)
    include(proto_utils)

    define_go_target(
            TARGET_NAME assfire-tsp-server-go
            DEPENDS assfire-tsp-proto-go
            GO_PACKAGE_NAME assfire.org/tsp/server
    )
endif()