file(GLOB_RECURSE CHART_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.yaml)

foreach(file ${CHART_FILES})
    configure_file(${file} ${file} @ONLY)
endforeach()