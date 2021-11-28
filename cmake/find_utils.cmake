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