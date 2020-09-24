function(GenerateGrpcReverseProxy)
    set(_options "")
    set(_singleargs TARGET SERVICE_DEFINITION PROTO_DEFINITION GO_ENTRY_POINT GO_MOD GO_MODULE_ROOT INSTALL_PREFIX)
    set(_multi_args "")
    cmake_parse_arguments(GenerateGrpcReverseProxy "${_options}" "${_singleargs}" "${_multi_args}" "${ARGN}")

    set(_TARGET ${GenerateGrpcReverseProxy_TARGET})
    set(_PROTO_DEFINITION ${GenerateGrpcReverseProxy_PROTO_DEFINITION})
    set(_SERVICE_DEFINITION ${GenerateGrpcReverseProxy_SERVICE_DEFINITION})
    set(_GO_ENTRY_POINT ${GenerateGrpcReverseProxy_GO_ENTRY_POINT})
    set(_GO_MOD ${GenerateGrpcReverseProxy_GO_MOD})
    set(_GO_MODULE_ROOT ${GenerateGrpcReverseProxy_GO_MODULE_ROOT})
    set(_INSTALL_PREFIX ${GenerateGrpcReverseProxy_INSTALL_PREFIX})

    message(STATUS "[gRPC gateway] Generating rules for gRPC reverse proxy generation")
    message(STATUS "[gRPC gateway] Target: ${_TARGET}")
    message(STATUS "[gRPC gateway] Proto definition: ${_PROTO_DEFINITION}")
    message(STATUS "[gRPC gateway] Service definition: ${_SERVICE_DEFINITION}")
    message(STATUS "[gRPC gateway] Go entrypoint implementation: ${_GO_ENTRY_POINT}")

    get_filename_component(_ABS_PROTO_DEFINITION ${_PROTO_DEFINITION} ABSOLUTE)
    get_filename_component(_ABS_SERVICE_DEFINITION ${_SERVICE_DEFINITION} ABSOLUTE)

    message(STATUS "[gRPC gateway] Proto definition (full): ${_ABS_PROTO_DEFINITION}")
    message(STATUS "[gRPC gateway] Service definition (full): ${_ABS_SERVICE_DEFINITION}")

    get_filename_component(_PROTO_WE ${_PROTO_DEFINITION} NAME_WE)
    set(_OUTPUT_FILENAME ${_PROTO_WE}.pb.gw.go)
    get_filename_component(_PROTO_DEFINITION_DIR ${_PROTO_DEFINITION} DIRECTORY)
    set(_FULL_OUTPUT_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${_PROTO_DEFINITION_DIR}/${_PROTO_WE}.pb.gw.go)
    set(_FULL_GO_OUTPUT_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${_PROTO_DEFINITION_DIR}/${_PROTO_WE}.pb.go)
    set(_FULL_SWAGGER_OUTPUT_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${_PROTO_DEFINITION_DIR}/${_PROTO_WE}.json)
    set(_FULL_ENTRY_POINT_OUTPUT_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${_GO_ENTRY_POINT})

    message(STATUS "[gRPC gateway] Output file: ${_FULL_OUTPUT_FILENAME}")

    add_custom_command(
            OUTPUT ${_FULL_GO_OUTPUT_FILENAME}
            COMMAND protobuf::protoc
            ARGS
            -I .
            --go_out ${CMAKE_CURRENT_BINARY_DIR}
            --go_opt paths=source_relative
            --go_opt plugins=grpc
            ${_PROTO_DEFINITION}
            DEPENDS ${_ABS_PROTO_DEFINITION}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating reverse proxy go source ${_FULL_GO_OUTPUT_FILENAME}"
            VERBATIM )

    add_custom_command(
            OUTPUT ${_FULL_OUTPUT_FILENAME}
            COMMAND protobuf::protoc
            ARGS
                -I .
                --grpc-gateway_out ${CMAKE_CURRENT_BINARY_DIR}
                --grpc-gateway_opt logtostderr=true
                --grpc-gateway_opt paths=source_relative
                --grpc-gateway_opt grpc_api_configuration=${_SERVICE_DEFINITION}
                ${_PROTO_DEFINITION}
            DEPENDS ${_ABS_PROTO_DEFINITION} ${_ABS_SERVICE_DEFINITION}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating reverse proxy source ${_FULL_OUTPUT_FILENAME}"
            VERBATIM )

    add_custom_command(
            OUTPUT ${_FULL_SWAGGER_OUTPUT_FILENAME}
            COMMAND protobuf::protoc
            ARGS
            -I .
            --swagger_out ${CMAKE_CURRENT_BINARY_DIR}
            --swagger_opt logtostderr=true
            --swagger_opt grpc_api_configuration=${_SERVICE_DEFINITION}
            ${_PROTO_DEFINITION}
            DEPENDS ${_FULL_OUTPUT_FILENAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating swagger API description ${_FULL_SWAGGER_OUTPUT_FILENAME}"
            VERBATIM )

    get_filename_component(_GO_ENTRY_POINT_NAME ${_GO_ENTRY_POINT} NAME)
    get_filename_component(_GO_MOD_NAME ${_GO_MOD} NAME)
    configure_file(${_GO_ENTRY_POINT} ${CMAKE_CURRENT_BINARY_DIR}/${_GO_MODULE_ROOT}/${_GO_ENTRY_POINT_NAME} COPYONLY)
    configure_file(${_GO_MOD} ${CMAKE_CURRENT_BINARY_DIR}/${_GO_MODULE_ROOT}/${_GO_MOD_NAME} COPYONLY)

    if(WIN32)
        set(_GO_OUTPUT_NAME ${_TARGET}-reverse-proxy.exe)
    else()
        set(_GO_OUTPUT_NAME ${_TARGET}-reverse-proxy)
    endif()

    add_custom_target(${_TARGET}-reverse-proxy-internal ALL
            COMMAND go build -o ${_GO_OUTPUT_NAME}
            WORKING_DIRECTORY ${_GO_MODULE_ROOT}
            DEPENDS ${_FULL_OUTPUT_FILENAME} ${_FULL_SWAGGER_OUTPUT_FILENAME} ${_FULL_GO_OUTPUT_FILENAME})
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${_GO_MODULE_ROOT}/${_GO_OUTPUT_NAME} DESTINATION ${_INSTALL_PREFIX})
endfunction()