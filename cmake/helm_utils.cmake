function(define_helm_target)

    set(_options "")
    set(_singleargs TARGET_NAME CHART_ROOT INSTALL_RELEASE_NAME)
    set(_multi_args CHART_FILES)
    cmake_parse_arguments(define_helm_target "${_options}" "${_singleargs}" "${_multi_args}" "${ARGN}")

    set(TARGET_NAME ${define_helm_target_TARGET_NAME})
    set(CHART_FILES ${define_helm_target_CHART_FILES})
    set(CHART_ROOT ${define_helm_target_CHART_ROOT})
    set(INSTALL_RELEASE_NAME ${define_helm_target_INSTALL_RELEASE_NAME})

    message(STATUS "[Helm] Generating helm target: ${TARGET_NAME}")
    message(VERBOSE "[Helm]   Chart root: ${CHART_ROOT}")

    set(PROCESSED_CHART_FILES "")
    foreach(file ${CHART_FILES})
        message(VERBOSE "[Helm]   Processing file: ${file} -> ${CMAKE_CURRENT_BINARY_DIR}/${file}")
        configure_file(${file} ${file} @ONLY)
        list(APPEND PROCESSED_CHART_FILED ${CMAKE_CURRENT_BINARY_DIR}/${file})

        get_filename_component(CHART_FILE_NAME ${file} NAME)
        if(CHART_FILE_NAME STREQUAL "Chart.yaml")
            file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/${file} chart_name_strings REGEX "^name: [a-zA-Z-]+")
            foreach(chart_name_string ${chart_name_strings})
                message(VERBOSE "[Helm]   Chart name string: ${chart_name_string}")
                string(REGEX REPLACE "^name: ([a-zA-Z-]+)" "\\1" CHART_NAME ${chart_name_string})
            endforeach()

            file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/${file} chart_version_strings REGEX "^version: ([a-zA-Z-\\.]+)")
            foreach(chart_version_string ${chart_version_strings})
                message(VERBOSE "[Helm]   Chart version string: ${chart_version_string}")
                string(REGEX REPLACE "^version: ([a-zA-Z-\\.]+)" "\\1" CHART_VERSION ${chart_version_string})
            endforeach()
        endif()
    endforeach()
    message(VERBOSE "[Helm]   Chart name: ${CHART_NAME}")
    message(VERBOSE "[Helm]   Chart version: ${CHART_VERSION}")

    include(find_utils)
    find_required_program(helm HELM_EXECUTABLE)
    find_required_program(curl CURL_EXECUTABLE)

    add_custom_target(${TARGET_NAME}
            DEPENDS ${PROCESSED_CHART_FILES}
            COMMAND ${HELM_EXECUTABLE} lint
            COMMAND ${HELM_EXECUTABLE} dependencies update
            COMMAND ${HELM_EXECUTABLE} package .
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CHART_ROOT}
            COMMENT "Packaging helm chart ${CMAKE_CURRENT_BINARY_DIR}/${CHART_ROOT} -> ${CHART_NAME}-${CHART_VERSION}.tgz"
            )

    add_custom_target(${TARGET_NAME}-upload
            DEPENDS ${TARGET_NAME}
            COMMAND if [ $$\(${CURL_EXECUTABLE} --request POST --user gitlab-ci-token:$$CI_JOB_TOKEN --form \"chart=@${CHART_NAME}-${CHART_VERSION}.tgz\" \"$ENV{CI_API_V4_URL}/projects/$ENV{CI_PROJECT_ID}/packages/helm/api/stable/charts\"\) != 201 ]; then false; fi
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CHART_ROOT}
            COMMENT "Uploading helm chart ${CHART_NAME}-${CHART_VERSION}.tgz to package repository $ENV{CI_API_V4_URL}/projects/$ENV{CI_PROJECT_ID}/packages/helm/api/stable/charts"
            )

    add_custom_target(${TARGET_NAME}-install
            DEPENDS ${TARGET_NAME}
            COMMAND ${HELM_EXECUTABLE} upgrade -i -n ${ASSFIRE_HELM_INSTALL_RELEASE_NAMESPACE} ${INSTALL_RELEASE_NAME} ${CHART_NAME}-${CHART_VERSION}.tgz
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CHART_ROOT}
            COMMENT "Installing helm chart ${CHART_NAME}-${CHART_VERSION}.tgz, release name: ${INSTALL_RELEASE_NAME} release namespace: ${ASSFIRE_HELM_INSTALL_RELEASE_NAMESPACE}"
            )

    add_custom_target(${TARGET_NAME}-uninstall
            COMMAND ${HELM_EXECUTABLE} uninstall -n ${ASSFIRE_HELM_INSTALL_RELEASE_NAMESPACE} ${INSTALL_RELEASE_NAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CHART_ROOT}
            COMMENT "Uninstalling helm release ${INSTALL_RELEASE_NAME}, release namespace: ${ASSFIRE_HELM_INSTALL_RELEASE_NAMESPACE}"
            )

    add_custom_target(${TARGET_NAME}-deploy
            DEPENDS ${TARGET_NAME}
            COMMAND ${HELM_EXECUTABLE} repo add --username $CI_REGISTRY_USER --password $CI_REGISTRY_PASSWORD assfire-suite $ENV{CI_API_V4_URL}/projects/$ENV{CI_PROJECT_ID}/packages/helm/api/stable
            COMMAND ${HELM_EXECUTABLE} upgrade -i -n ${ASSFIRE_HELM_INSTALL_RELEASE_NAMESPACE} ${INSTALL_RELEASE_NAME} assfire-suite/${CHART_NAME} --version "${ASSFIRE_APPLICATION_RELEASE_VERSION}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CHART_ROOT}
            COMMENT "Uninstalling helm release ${INSTALL_RELEASE_NAME}, release namespace: ${ASSFIRE_HELM_INSTALL_RELEASE_NAMESPACE}"
            )

    add_dependencies(helm-package ${TARGET_NAME})
    add_dependencies(helm-push ${TARGET_NAME}-upload)
    add_dependencies(helm-install ${TARGET_NAME}-install)
    add_dependencies(helm-uninstall ${TARGET_NAME}-uninstall)
    add_dependencies(helm-deploy ${TARGET_NAME}-deploy)

endfunction()

