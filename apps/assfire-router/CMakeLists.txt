set(ASSFIRE_ROUTER_APP_NAME assfire-router)

if (ASSFIRE_BUNDLE_INSTALL_MODE)
    set(ASSFIRE_ROUTER_INSTALL_PREFIX ${ASSFIRE_BUNDLE_INSTALL_PREFIX}/${ASSFIRE_ROUTER_APP_NAME})
endif ()

add_subdirectory(grpc)
add_subdirectory(server)
add_subdirectory(client)
