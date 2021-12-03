function(find_required_program PROGRAM_NAME VAR_NAME)
    find_program(${VAR_NAME} ${PROGRAM_NAME})

    if(${VAR_NAME} STREQUAL "${VAR_NAME}-NOTFOUND")
        message(FATAL_ERROR "${PROGRAM_NAME} executable not found")
    endif()
    message(VERBOSE "Found ${PROGRAM_NAME} executable: ${${VAR_NAME}}")
endfunction()

function(find_optional_program PROGRAM_NAME VAR_NAME)
    find_program(${VAR_NAME} ${PROGRAM_NAME})

    if(${VAR_NAME} STREQUAL "${VAR_NAME}-NOTFOUND")
        message(WARNING "${PROGRAM_NAME} executable not found")
    endif()
    message(VERBOSE "Found ${PROGRAM_NAME} executable: ${${VAR_NAME}}")
endfunction()

macro(find_cpp_redis_vcpkg)
    if(NOT cpp_redis_included)
        find_path(TACOPIE_INCLUDE_DIR tacopie/tacopie)
        find_library(TACOPIE_LIBRARY tacopie)

        add_library(tacopie::tacopie INTERFACE IMPORTED)
        set_target_properties(tacopie::tacopie PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${TACOPIE_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "${TACOPIE_LIBRARY}"
                )

        find_path(REDIS_INCLUDE_DIR cpp_redis/cpp_redis)
        find_library(REDIS_LIBRARY cpp_redis)

        add_library(cpp_redis::cpp_redis INTERFACE IMPORTED)
        set_target_properties(cpp_redis::cpp_redis PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${REDIS_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "${REDIS_LIBRARY}"
                )

        target_link_libraries(cpp_redis::cpp_redis INTERFACE tacopie::tacopie)

        set(cpp_redis_included TRUE)
    endif()
endmacro()