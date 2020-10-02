if(ASSFIRE_APPS_BUILD_SAMPLE OR ASSFIRE_APPS_BUILD_ALL)
    include(name_utils)
    include(app_utils)

    define_common_vars(sample)

    add_subdirectory(api)
#    add_subdirectory(client)
#    add_subdirectory(engine)
#    add_subdirectory(gateway)
#    add_subdirectory(helm)
#    add_subdirectory(server)
#    add_subdirectory(tools)
endif()