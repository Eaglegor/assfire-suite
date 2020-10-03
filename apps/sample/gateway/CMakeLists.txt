if (ASSFIRE_GENERATE_GRPC_GATEWAYS)

    find_package(protobuf REQUIRED)

    set(PROTOS
            assfire/api/v1/model/sample/sample.proto
            assfire/api/v1/service/sample/service.proto)
    set(IMPORT_DIRS
            ${CMAKE_SOURCE_DIR}/apps/sample/api
            ${CMAKE_SOURCE_DIR}/api-model
            ${GOOGLE_PROTOBUF_IMPORT_DIR})
    set(GO_OUT_DIR
            ${CMAKE_BINARY_DIR}/api/go/assfire-sample-grpc-gateway)
    file(MAKE_DIRECTORY ${GO_OUT_DIR})
    set(SWAGGER_OUT_DIR
            ${CMAKE_BINARY_DIR}/api/swagger)
    file(MAKE_DIRECTORY ${SWAGGER_OUT_DIR})
    set(GO_FILE
            ${GO_OUT_DIR}/assfire/api/v1/service/sample/service.pb.go)
    set(GO_GW_FILE
            ${GO_OUT_DIR}/assfire/api/v1/service/sample/service.pb.gw.go)
    set(SWAGGER_FILE
            ${GO_OUT_DIR}/assfire/api/v1/service/sample/service.json)

    list(TRANSFORM IMPORT_DIRS PREPEND --proto_path=)

    file(MAKE_DIRECTORY ${GO_OUT_DIR}/assfire.org)
    configure_file(assfire-sample-grpc-proxy.go ${GO_OUT_DIR}/assfire.org @ONLY)
    configure_file(go.mod ${GO_OUT_DIR}/assfire.org @ONLY)

    add_custom_target(assfire-sample-grpc-gateway ALL
            COMMAND ${GO_EXECUTABLE} build
            -o ${CMAKE_CURRENT_BINARY_DIR}/assfire-sample-grpc-gateway${CMAKE_EXECUTABLE_SUFFIX}
#            -pkgdir ${GO_OUT_DIR}
            WORKING_DIRECTORY ${GO_OUT_DIR}/assfire.org
            DEPENDS ${GO_FILE} ${GO_GW_FILE} ${SWAGGER_FILE})
    add_dependencies(assfire-sample-grpc-gateway assfire-sample-api)

    add_custom_command(
            OUTPUT ${GO_FILE}
            COMMAND protobuf::protoc
            ARGS
            ${IMPORT_DIRS}
            --go_out ${GO_OUT_DIR}
#            --go_opt paths=source_relative
            --go_opt plugins=grpc
            ${PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating go code for: ${PROTOS}"
            VERBATIM)

    add_custom_command(
            OUTPUT ${GO_GW_FILE}
            COMMAND protobuf::protoc
            ARGS
            ${IMPORT_DIRS}
#            --grpc-gateway_opt paths=source_relative
            --grpc-gateway_out ${GO_OUT_DIR}
            ${PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating grpc gateway code for: ${PROTOS}"
            VERBATIM)

    add_custom_command(
            OUTPUT ${SWAGGER_FILE}
            COMMAND protobuf::protoc
            ARGS
            ${IMPORT_DIRS}
            --swagger_out ${SWAGGER_OUT_DIR}
            ${PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating grpc gateway code for: ${PROTOS}"
            VERBATIM)


    #    add_custom_target(assfire-sample-grpc-gateway ALL
    #                        COMMAND ${GO_EXECUTABLE} build
    #                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    #                        DEPENDS ${_FULL_OUTPUT_FILENAME} ${_FULL_SWAGGER_OUTPUT_FILENAME} ${_FULL_GO_OUTPUT_FILENAME})

    #    add_custom_command(
    #            OUTPUT ${_FULL_OUTPUT_FILENAME}
    #            COMMAND protobuf::protoc
    #            ARGS
    #            -I .
    #            --grpc-gateway_out ${_OUTPUT_DIR}
    #            --grpc-gateway_opt logtostderr=true
    #            --grpc-gateway_opt paths=source_relative
    #            --grpc-gateway_opt grpc_api_configuration=${_SERVICE_DEFINITION}
    #            ${_PROTO_DEFINITION}
    #            DEPENDS ${_ABS_PROTO_DEFINITION} ${_ABS_SERVICE_DEFINITION}
    #            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #            COMMENT "Generating reverse proxy source ${_FULL_OUTPUT_FILENAME}"
    #            VERBATIM)
    #
    #    add_custom_command(
    #            OUTPUT ${_FULL_SWAGGER_OUTPUT_FILENAME}
    #            COMMAND protobuf::protoc
    #            ARGS
    #            -I .
    #            --swagger_out ${_OUTPUT_DIR}
    #            --swagger_opt logtostderr=true
    #            --swagger_opt grpc_api_configuration=${_SERVICE_DEFINITION}
    #            ${_PROTO_DEFINITION}
    #            DEPENDS ${_FULL_OUTPUT_FILENAME}
    #            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #            COMMENT "Generating swagger API description ${_FULL_SWAGGER_OUTPUT_FILENAME}"
    #            VERBATIM)
    #
    #    get_filename_component(_GO_ENTRY_POINT_NAME ${_GO_ENTRY_POINT} NAME)
    #    get_filename_component(_GO_MOD_NAME ${_GO_MOD} NAME)
    #    configure_file(${_GO_ENTRY_POINT} ${CMAKE_CURRENT_BINARY_DIR}/${_GO_MODULE_ROOT}/${_GO_ENTRY_POINT_NAME} @ONLY)
    #    configure_file(${_GO_MOD} ${CMAKE_CURRENT_BINARY_DIR}/${_GO_MODULE_ROOT}/${_GO_MOD_NAME} COPYONLY)
    #
    #    if (WIN32)
    #        set(_GO_OUTPUT_NAME ${_TARGET_NAME}.exe)
    #    else ()
    #        set(_GO_OUTPUT_NAME ${_TARGET_NAME})
    #    endif ()
    #
    #
    #    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${_GO_MODULE_ROOT}/${_GO_OUTPUT_NAME} COMPONENT ${_COMPONENT_NAME} DESTINATION ${_INSTALL_PREFIX})
    #    if (ASSFIRE_PACKAGE_BUILD_RPM)
    #        cpack_add_component(${_COMPONENT_NAME})
    #    endif ()

endif ()