if (ASSFIRE_BUNDLE_INSTALL_MODE)
    set(ROUTES_PROVIDER_SERVER_BUNDLE_PREFIX "assfire-router")
endif ()

set(ROUTES_PROVIDER_SERVER_INSTALL_PREFIX ${ROUTES_PROVIDER_SERVER_BUNDLE_PREFIX}/bin)

add_subdirectory(grpc)
add_subdirectory(server)
add_subdirectory(client)
