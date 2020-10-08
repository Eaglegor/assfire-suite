if (ASSFIRE_GENERATE_GRPC_GATEWAYS)

    find_package(protobuf REQUIRED)

    set(MODEL_PROTOS
            assfire/api/v1/model/routing/routing.proto
            assfire/api/v1/model/optimization/transport/transport.proto)
    set(SERVICE_PROTOS
            assfire/api/v1/service/tsp/tsp.proto)
    set(IMPORT_DIRS
            ${CMAKE_SOURCE_DIR}/apps/tsp/api
            ${CMAKE_SOURCE_DIR}/api-model
            ${GOOGLE_PROTOBUF_IMPORT_DIR})
    set(GO_OUT_DIR
            ${CMAKE_CURRENT_BINARY_DIR})
    file(MAKE_DIRECTORY ${GO_OUT_DIR})
    set(SWAGGER_OUT_DIR
            ${CMAKE_CURRENT_BINARY_DIR}/swagger)
    file(MAKE_DIRECTORY ${SWAGGER_OUT_DIR})
    set(GO_FILE
            ${GO_OUT_DIR}/assfire/api/v1/service/tsp/tsp.pb.go)
    set(GO_GW_FILE
            ${GO_OUT_DIR}/assfire/api/v1/service/tsp/tsp.pb.gw.go)
    set(SWAGGER_FILE
            ${GO_OUT_DIR}/assfire/api/v1/service/tsp/tsp.json)

    list(TRANSFORM IMPORT_DIRS PREPEND --proto_path=)

    file(MAKE_DIRECTORY ${GO_OUT_DIR}/assfire.org)
    configure_file(assfire-tsp-grpc-proxy.go ${GO_OUT_DIR}/assfire.org @ONLY)
    configure_file(go.mod ${GO_OUT_DIR}/assfire.org @ONLY)

    add_custom_target(assfire-tsp-grpc-gateway ALL
            COMMAND ${GO_EXECUTABLE} build
            -o ${CMAKE_CURRENT_BINARY_DIR}/assfire-tsp-grpc-gateway${CMAKE_EXECUTABLE_SUFFIX}
            WORKING_DIRECTORY ${GO_OUT_DIR}/assfire.org
            DEPENDS ${GO_FILE} ${GO_GW_FILE} ${SWAGGER_FILE})
    add_dependencies(assfire-tsp-grpc-gateway assfire-tsp-api)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/assfire-tsp-grpc-gateway${CMAKE_EXECUTABLE_SUFFIX} COMPONENT tsp-grpc-gateway DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    if(ASSFIRE_PACKAGE_BUILD_RPM)
        cpack_add_component(tsp-grpc-gateway)
    endif()

    add_custom_command(
            OUTPUT ${GO_FILE}
            COMMAND protobuf::protoc
            ARGS
            ${IMPORT_DIRS}
            --go_out ${GO_OUT_DIR}
            --go_opt plugins=grpc
            ${MODEL_PROTOS} ${SERVICE_PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating go code for: ${MODEL_PROTOS} ${SERVICE_PROTOS}"
            VERBATIM)

    add_custom_command(
            OUTPUT ${GO_GW_FILE}
            COMMAND protobuf::protoc
            ARGS
            ${IMPORT_DIRS}
            --grpc-gateway_out ${GO_OUT_DIR}
            ${SERVICE_PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating grpc gateway code for: ${SERVICE_PROTOS}"
            VERBATIM)

    add_custom_command(
            OUTPUT ${SWAGGER_FILE}
            COMMAND protobuf::protoc
            ARGS
            ${IMPORT_DIRS}
            --swagger_out ${SWAGGER_OUT_DIR}
            ${SERVICE_PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating swagger API definition for: ${SERVICE_PROTOS}"
            VERBATIM)
endif ()

