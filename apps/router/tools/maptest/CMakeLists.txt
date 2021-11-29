if(ASSFIRE_ROUTER_MAPTEST_ENABLED)
    set(FILES
            public/*
            src/*
            nginx.conf
            package.json
            package-lock.json)
    set(TARGET_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/react)

    file(GLOB_RECURSE PROJECT_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${FILES})

    foreach (file ${PROJECT_FILES})
        configure_file(${file} ${TARGET_DIRECTORY}/${file} @ONLY)
    endforeach ()

    find_required_program(npm NPM_EXECUTABLE)
    add_custom_target(assfire-router-maptest ALL
            COMMAND ${NPM_EXECUTABLE} install
            COMMAND ${NPM_EXECUTABLE} run build
            WORKING_DIRECTORY ${TARGET_DIRECTORY}
            COMMENT "[Tools][React] Building assfire router maptest application"
            )

    if(ASSFIRE_DOCKER_ENABLED)
        include(docker_utils)
        define_docker_target(
                TARGET_NAME assfire-router-maptest-docker
                DOCKERFILE maptest.Dockerfile
                IMAGE_NAME assfire-router-maptest
                IMAGE_TAG ${ASSFIRE_APPLICATION_RELEASE_VERSION}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/apps/router/tools/maptest/react
        )
    endif()
endif()