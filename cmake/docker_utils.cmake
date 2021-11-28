function(define_docker_target)
    set(_options "")
    set(_singleargs TARGET_NAME DOCKERFILE IMAGE_NAME IMAGE_TAG WORKING_DIRECTORY)
    set(_multi_args "")
    cmake_parse_arguments(define_docker_target "${_options}" "${_singleargs}" "${_multi_args}" "${ARGN}")

    set(TARGET_NAME ${define_docker_target_TARGET_NAME})
    set(DOCKERFILE ${define_docker_target_DOCKERFILE})
    set(IMAGE_NAME ${define_docker_target_IMAGE_NAME})
    set(IMAGE_TAG ${define_docker_target_IMAGE_TAG})
    set(WORKING_DIRECTORY ${define_docker_target_WORKING_DIRECTORY})

    message(STATUS "[Docker] Generating docker target ${TARGET_NAME}")
    message(VERBOSE "[Docker]   Dockerfile: ${DOCKERFILE}")
    message(VERBOSE "[Docker]   Image name: ${IMAGE_NAME}")
    message(VERBOSE "[Docker]   Image tag: ${IMAGE_TAG}")
    message(VERBOSE "[Docker]   Working directory: ${WORKING_DIRECTORY}")

    configure_file(${DOCKERFILE} ${DOCKERFILE})

    include(find_utils)
    find_required_program(docker DOCKER_EXECUTABLE)

    add_custom_target(${TARGET_NAME}
            DEPENDS ${DOCKERFILE}
            COMMAND ${DOCKER_EXECUTABLE} build . -f ${CMAKE_CURRENT_BINARY_DIR}/${DOCKERFILE} -t $ENV{CI_REGISTRY}/assfire/assfire-suite/${IMAGE_NAME}:${IMAGE_TAG}
            WORKING_DIRECTORY ${WORKING_DIRECTORY}
            COMMENT "Building docker image $ENV{CI_REGISTRY}/assfire/assfire-suite/${IMAGE_NAME}:${IMAGE_TAG} from ${DOCKERFILE}"
            )

    add_custom_target(${TARGET_NAME}-upload
            DEPENDS ${TARGET_NAME}
            COMMAND ${DOCKER_EXECUTABLE} login -u $ENV{CI_REGISTRY_USER} -p $ENV{CI_REGISTRY_PASSWORD} $ENV{CI_REGISTRY}
            COMMAND ${DOCKER_EXECUTABLE} push $ENV{CI_REGISTRY}/assfire/assfire-suite/${IMAGE_NAME}:${IMAGE_TAG}
            COMMENT "Uploading docker image $ENV{CI_REGISTRY}/assfire/assfire-suite/${IMAGE_NAME}:${IMAGE_TAG}"
            )

    add_dependencies(docker-build ${TARGET_NAME})
    add_dependencies(docker-push ${TARGET_NAME}-upload)

endfunction()