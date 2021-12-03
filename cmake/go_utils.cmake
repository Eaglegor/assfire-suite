function(define_go_executable_target)
    set(_options "")
    set(_singleargs TARGET_NAME GO_PACKAGE_NAME GO_MOD_PATH RPM_COMPONENT_NAME)
    set(_multiargs DEPENDS REQUIRES)
    cmake_parse_arguments(define_go_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_go_target_TARGET_NAME})
    set(DEPENDS ${define_go_target_DEPENDS})
    set(REQUIRES ${define_go_target_REQUIRES})
    set(GO_PACKAGE_NAME ${define_go_target_GO_PACKAGE_NAME})
    set(GO_MOD_PATH ${define_go_target_GO_MOD_PATH})
    set(RPM_COMPONENT_NAME ${define_go_target_RPM_COMPONENT_NAME})

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_TRANSITIVE_DEPS ${dep} DEPENDS_ON)
        if(NOT ${dep}_TRANSITIVE_DEPS STREQUAL "${dep}_TRANSITIVE_DEPS-NOTFOUND")
            list(APPEND TRANSITIVE_DEPS ${${dep}_TRANSITIVE_DEPS})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES TRANSITIVE_DEPS)

    list(APPEND REQUIRES "google.golang.org/protobuf v1.27.1")

    message(STATUS "[Server][Go] Generating go server target: ${TARGET_NAME}")
    message(VERBOSE "[Server][Go]   Depends on: ${DEPENDS}")
    message(VERBOSE "[Server][Go]   Go package name: ${GO_PACKAGE_NAME}")
    message(VERBOSE "[Server][Go]   Requires: ${REQUIRES}")
    message(VERBOSE "[Server][Go]   Transitive dependencies: ${TRANSITIVE_DEPS}")

    if(NOT GO_MOD_PATH)
        set(GO_MOD_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(REQUIRE_DIRECTIVES "")
    set(REPLACE_DIRECTIVES "")
    foreach(req ${REQUIRES})
        string(APPEND REQUIRE_DIRECTIVES "${req}\n")
    endforeach()
    foreach(req ${DEPENDS})
        get_target_property(DEP_MODULE_NAME ${req} GO_PACKAGE_NAME)
        get_target_property(DEP_MODULE_PATH ${req} GO_MODULE_PATH)

        if(NOT DEP_MODULE_NAME STREQUAL "DEP_MODULE_NAME-NOTFOUND" AND NOT DEP_MODULE_PATH STREQUAL "DEP_MODULE_PATH-NOTFOUND")
            string(APPEND REQUIRE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION}\n")
            string(APPEND REPLACE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION} => ${DEP_MODULE_PATH}\n")
        endif()
    endforeach()
    foreach(req ${TRANSITIVE_DEPS})
        get_target_property(DEP_MODULE_NAME ${req} GO_PACKAGE_NAME)
        get_target_property(DEP_MODULE_PATH ${req} GO_MODULE_PATH)
        if(NOT DEP_MODULE_NAME STREQUAL "DEP_MODULE_NAME-NOTFOUND" AND NOT DEP_MODULE_PATH STREQUAL "DEP_MODULE_PATH-NOTFOUND")
            string(APPEND REQUIRE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION}\n")
            string(APPEND REPLACE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION} => ${DEP_MODULE_PATH}\n")
        endif()
    endforeach()
    message(VERBOSE "[Server][Go] Resolved go.mod require directives: ${REQUIRE_DIRECTIVES}")
    message(VERBOSE "[Server][Go] Resolved go.mod replace directives: ${REPLACE_DIRECTIVES}")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/go.mod ${GO_MOD_PATH} @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/go.sum ${GO_MOD_PATH} @ONLY)

    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/dev.gitignore ${GO_MOD_PATH}/.gitignore)

    include(find_utils)
    find_required_program(go GO_EXECUTABLE)

    add_custom_target(${TARGET_NAME} ALL
            COMMAND ${GO_EXECUTABLE} install
            COMMAND ${GO_EXECUTABLE} build -o ${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}
            WORKING_DIRECTORY ${GO_MOD_PATH}
            DEPENDS ${DEPENDS} ${GO_MOD_PATH}/go.sum ${GO_MOD_PATH}/go.mod)

    set_target_properties(${TARGET_NAME} PROPERTIES PROTO_IMPORT_DIRS "${FINAL_IMPORT_DIRS}")
    set_target_properties(${TARGET_NAME} PROPERTIES GO_MODULE_PATH "${GO_MOD_PATH}")
    set_target_properties(${TARGET_NAME} PROPERTIES GO_PACKAGE_NAME "${GO_PACKAGE_NAME}")
    set_target_properties(${TARGET_NAME} PROPERTIES DEPENDS_ON "${DEPENDS}")
    set_target_properties(${TARGET_NAME} PROPERTIES TRANSITIVE_DEPS "${TRANSITIVE_DEPS}")

    install(PROGRAMS ${GO_MOD_PATH}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX} TYPE BIN COMPONENT ${RPM_COMPONENT_NAME})
    if(ASSFIRE_RPM_ENABLED)
        cpack_add_component(${RPM_COMPONENT_NAME})
    endif()
endfunction()
