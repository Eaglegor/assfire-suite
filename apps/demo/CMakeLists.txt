if (ASSFIRE_DEMO_APPS_ENABLED)
    set(FILES public/*
            src/*)
    set(TARGET_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/vue_package)

    file(GLOB_RECURSE GLOB_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS ${FILES})
    set(PROJECT_FILES ${GLOB_FILES}
            nginx.conf
            package.json
            package-lock.json
            babel.config.js
    )

    foreach (file ${PROJECT_FILES})
        configure_file(${file} ${TARGET_DIRECTORY}/${file} @ONLY)
    endforeach ()

    find_required_program(npm NPM_EXECUTABLE)
    add_custom_target(assfire-demo ALL
            COMMAND ${NPM_EXECUTABLE} install
            COMMAND ${NPM_EXECUTABLE} run build
            WORKING_DIRECTORY ${TARGET_DIRECTORY}
            COMMENT "[Tools][Demo] Building assfire demo application"
            )

    if (ASSFIRE_DOCKER_ENABLED)
        include(docker_utils)
        define_docker_target(
                TARGET_NAME assfire-demo-docker
                DOCKERFILE Dockerfile
                IMAGE_NAME assfire-demo
                IMAGE_TAG ${ASSFIRE_APPLICATION_RELEASE_VERSION}
                WORKING_DIRECTORY ${TARGET_DIRECTORY}
        )
    endif ()
endif ()