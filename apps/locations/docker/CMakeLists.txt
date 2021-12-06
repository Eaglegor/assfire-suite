if(ASSFIRE_DOCKER_ENABLED)
    include(docker_utils)
    define_docker_target(
            TARGET_NAME assfire-locations-server-docker
            DOCKERFILE Dockerfile
            IMAGE_NAME assfire-locations-server
            IMAGE_TAG ${ASSFIRE_APPLICATION_RELEASE_VERSION}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
