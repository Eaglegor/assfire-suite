function(prepare_helm_chart)

    set(_options "")
    set(_singleargs CHART_ROOT)
    set(_multi_args "")
    cmake_parse_arguments(prepare_helm_chart "${_options}" "${_singleargs}" "${_multi_args}" "${ARGN}")

    set(_CHART_ROOT ${prepare_helm_chart_CHART_ROOT})
    set(_TARGET_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/helm_charts/${_CHART_ROOT})

    message(STATUS "[HELM] Preparing helm chart: ${CMAKE_CURRENT_SOURCE_DIR}/${_CHART_ROOT}")
    message(STATUS "[HELM] Target directory: ${_TARGET_DIRECTORY}")


    file(GLOB_RECURSE _CHART_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${_CHART_ROOT}" "${CMAKE_CURRENT_SOURCE_DIR}/${_CHART_ROOT}/*")

    foreach(file ${_CHART_FILES})
        message(STATUS "[HELM] Processing file: ${CMAKE_CURRENT_SOURCE_DIR}/${_CHART_ROOT}/${file} -> ${_TARGET_DIRECTORY}/${file}")
        configure_file(${_CHART_ROOT}/${file} ${_TARGET_DIRECTORY}/${file} @ONLY)
    endforeach()

endfunction()