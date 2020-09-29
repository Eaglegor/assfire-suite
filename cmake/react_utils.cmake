function(configure_react_project)

    set(_options "")
    set(_singleargs PROJECT_ROOT)
    set(_multi_args FILES)
    cmake_parse_arguments(configure_react_project "${_options}" "${_singleargs}" "${_multi_args}" "${ARGN}")

    set(_PROJECT_ROOT ${configure_react_project_PROJECT_ROOT})
    set(_TARGET_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/react/${_PROJECT_ROOT})
    set(_FILES ${configure_react_project_FILES})

    message(STATUS "[REACT] Configuring react project: ${CMAKE_CURRENT_SOURCE_DIR}/${_PROJECT_ROOT}")
    message(STATUS "[REACT] Target directory: ${_TARGET_DIRECTORY}")
    message(STATUS "[REACT] File patterns: ${_FILES}")

    file(GLOB_RECURSE _PROJECT_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${_PROJECT_ROOT}" ${_FILES})

    foreach(file ${_PROJECT_FILES})
        message(STATUS "[REACT] Processing file: ${CMAKE_CURRENT_SOURCE_DIR}/${_PROJECT_ROOT}/${file} -> ${_TARGET_DIRECTORY}/${file}")
        configure_file(${_PROJECT_ROOT}/${file} ${_TARGET_DIRECTORY}/${file} @ONLY)
    endforeach()
endfunction()