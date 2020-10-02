function(assfire_define_api component_name)
    find_package(protobuf CONFIG REQUIRED)
    find_package(gRPC CONFIG REQUIRED)
    get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)

    set(_options "")
    set(_singleargs COMPONENT)
    set(_multi_args PROTOS)
    cmake_parse_arguments(assfire_define_api "${_options}" "${_singleargs}" "${_multi_args}" "${ARGN}")

    message(STATUS "Defining api for component \"${assfire_define_api_COMPONENT}\"")
    message(STATUS "Protos: ${assfire_define_api_PROTOS}")

    set(_UPPER_COMPONENT_NAME SAMPLE)
    #string(TOUPPER ${assfire_define_api_COMPONENT_NAME} _UPPER_COMPONENT_NAME)
    add_library(${ASSFIRE_${_UPPER_COMPONENT_NAME}_API_NAME})

    protobuf_generate(
            LANGUAGE cpp
            TARGET ${ASSFIRE_${_UPPER_COMPONENT_NAME}_API_NAME}
            PROTOS ${assfire_define_api_PROTOS}
            IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/api-model
            PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/api
    )

    protobuf_generate(
            LANGUAGE grpc
            PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
            PROTOS ${assfire_define_api_PROTOS}
            TARGET ${ASSFIRE_${_UPPER_COMPONENT_NAME}_API_NAME}
            GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
            IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/api-model
            PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/api
    )

    #target_include_directories(${ASSFIRE_${_UPPER_COMPONENT_NAME}_API_NAME} PUBLIC ${CMAKE_BINARY_DIR}/api)

    target_link_libraries(${ASSFIRE_${_UPPER_COMPONENT_NAME}_API_NAME} PUBLIC gRPC::grpc++ protobuf::libprotobuf assfire-api-model)
endfunction()
