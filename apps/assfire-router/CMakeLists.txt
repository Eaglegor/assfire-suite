include(name_utils)
define_common_names(ASSFIRE_ROUTER assfire-router)

if (ASSFIRE_BUNDLE_INSTALL_MODE)
    set(ASSFIRE_ROUTER_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${ASSFIRE_ROUTER_APP_NAME})
else()
    set(ASSFIRE_ROUTER_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif ()

add_subdirectory(grpc)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(helm)
