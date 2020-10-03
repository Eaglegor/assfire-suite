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
            ${CMAKE_CURRENT_BINARY_DIR})
    file(MAKE_DIRECTORY ${GO_OUT_DIR})
    set(SWAGGER_OUT_DIR
            ${CMAKE_CURRENT_BINARY_DIR}/swagger)
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
            WORKING_DIRECTORY ${GO_OUT_DIR}/assfire.org
            DEPENDS ${GO_FILE} ${GO_GW_FILE} ${SWAGGER_FILE})
    add_dependencies(assfire-sample-grpc-gateway assfire-sample-api)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/assfire-sample-grpc-gateway${CMAKE_EXECUTABLE_SUFFIX} COMPONENT sample-grpc-gateway DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    if(ASSFIRE_PACKAGE_BUILD_RPM)
        cpack_add_component(sample-grpc-gateway)
    endif()

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
endif ()

