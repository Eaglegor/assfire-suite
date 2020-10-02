include(name_utils)
define_common_vars(router)

if (ASSFIRE_BUNDLE_INSTALL_MODE)
    set(ASSFIRE_ROUTER_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${ASSFIRE_ROUTER_APP_NAME})
else()
    set(ASSFIRE_ROUTER_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif ()

add_subdirectory(gateway)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tools)
add_subdirectory(helm)