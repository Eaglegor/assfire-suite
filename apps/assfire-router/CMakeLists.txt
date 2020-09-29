include(name_utils)
define_common_names(router)

if (${PROJECT_PREFIX}_BUNDLE_INSTALL_MODE)
    set(${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${${PROJECT_PREFIX}_ROUTER_APP_NAME})
else()
    set(${PROJECT_PREFIX}_ROUTER_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif ()

add_subdirectory(grpc)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tools)
add_subdirectory(helm)