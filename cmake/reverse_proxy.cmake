function(generate_grpc_gateway_endpoints_source)
    set(_options "")
    set(_singleargs OUT_DIR)
    set(_multiargs SERVICES GO_PACKAGE_NAME)
    cmake_parse_arguments(generate_grpc_gateway_endpoints_source "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(OUT_DIR ${generate_grpc_gateway_endpoints_source_OUT_DIR})
    set(SERVICES ${generate_grpc_gateway_endpoints_source_SERVICES})
    set(ENABLE_CORS ${ASSFIRE_ALLOW_CORS_IN_GRPC_GATEWAY})
    set(GO_PACKAGE_NAME ${generate_grpc_gateway_endpoints_source_GO_PACKAGE_NAME})

    set(ASSFIRE_GO_PACKAGE_NAME ${GO_PACKAGE_NAME})

    foreach(service ${SERVICES})
        string(APPEND ASSFIRE_REGISTER_GRPC_SERVICE_ENDPOINTS "
  err := gw.Register${service}HandlerFromEndpoint(ctx, mux,  *grpcServerEndpoint, opts)
  if err != nil {
   return err
  }
        ")
    endforeach()

    set(ASSFIRE_GRPC_GATEWAY_MODULE ${GO_PACKAGE_NAME})

    message(DEBUG "Generated endpoints: ${ASSFIRE_REGISTER_GRPC_SERVICE_ENDPOINTS}")

    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/assfire-grpc-proxy.go ${OUT_DIR} @ONLY)
endfunction()


function(define_go_grpc_proxy_target)
    set(_options "")
    set(_singleargs TARGET_NAME RPM_COMPONENT_NAME GO_SUBPACKAGE_NAME GO_PACKAGE_PREFIX)
    set(_multiargs DEPENDS PROTO_IMPORT_DIRS PROTOS)
    cmake_parse_arguments(define_go_grpc_proxy_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_go_grpc_proxy_target_TARGET_NAME})
    set(DEPENDS ${define_go_grpc_proxy_target_DEPENDS})
    set(PROTO_IMPORT_DIRS ${define_go_grpc_proxy_target_PROTO_IMPORT_DIRS})
    set(PROTOS ${define_go_grpc_proxy_target_PROTOS})
    set(GO_SUBPACKAGE_NAME ${define_go_grpc_proxy_target_GO_SUBPACKAGE_NAME})
    set(RPM_COMPONENT_NAME ${define_go_grpc_proxy_target_RPM_COMPONENT_NAME})
    set(GO_PACKAGE_PREFIX ${define_go_grpc_proxy_target_GO_PACKAGE_PREFIX})

    message(STATUS "[Gateway][Go] Generating grpc-gateway target: ${TARGET_NAME}")
    message(VERBOSE "[Gateway][Go]   Depends on: ${DEPENDS}")
    message(VERBOSE "[Gateway][Go]   Additional proto import dirs: ${PROTO_IMPORT_DIRS}")
    message(VERBOSE "[Gateway][Go]   Protos: ${PROTOS}")

    find_package(protobuf CONFIG REQUIRED)
    get_target_property(GOOGLE_PROTOBUF_IMPORT_DIR protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

    list(APPEND IMPORTED_IMPORT_DIRS ${GOOGLE_PROTOBUF_IMPORT_DIR})

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_IMPORTED_DIRS ${dep} PROTO_IMPORT_DIRS)
        list(APPEND IMPORTED_IMPORT_DIRS ${${dep}_IMPORTED_DIRS})
    endforeach()

    set(FINAL_IMPORT_DIRS ${PROTO_IMPORT_DIRS} ${IMPORTED_IMPORT_DIRS})
    list(REMOVE_DUPLICATES FINAL_IMPORT_DIRS)

    message(VERBOSE "[Gateway][Go]   Summarized import dirs: ${FINAL_IMPORT_DIRS}")

    generate_protobuf_include_strings(PROTO_INCLUDE_STRINGS "${FINAL_IMPORT_DIRS}")

    foreach(proto ${PROTOS})
        find_file(full_proto_path ${proto} PATHS ${FINAL_IMPORT_DIRS} REQUIRED)
        list(APPEND FULL_PROTOS "${full_proto_path}")
        unset(full_proto_path CACHE)
        string(REPLACE ".proto" ".pb.gw.go" proto_gw ${proto})
        list(APPEND GO_GW_SOURCE_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${proto_gw}")
        string(REPLACE ".proto" ".json" proto_sw ${proto})
        list(APPEND GO_SW_SOURCE_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${proto_sw}")
    endforeach()

    foreach(proto_file ${FULL_PROTOS})
        file(STRINGS ${proto_file} proto_file_contents REGEX ".*service[ ]+[a-zA-Z0-9]+.*")
        foreach(service_string ${proto_file_contents})
            string(REGEX REPLACE ".*service[ ]+([a-zA-Z0-9]+).*" "\\1" service_name ${service_string})
            list(APPEND SERVICE_NAMES ${service_name})
        endforeach()
    endforeach()

    message(DEBUG "Parsed service names: ${SERVICE_NAMES}")

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/router)

    generate_grpc_gateway_endpoints_source(
            OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
            SERVICES "${SERVICE_NAMES}"
            GO_PACKAGE_NAME "${GO_SUBPACKAGE_NAME}"
    )

    add_custom_command(
            OUTPUT ${GO_GW_SOURCE_OUTPUTS}
            COMMAND protobuf::protoc
            ARGS
            ${PROTO_INCLUDE_STRINGS}
            --grpc-gateway_out ${CMAKE_CURRENT_BINARY_DIR}
            --grpc-gateway_opt standalone=true
            --grpc-gateway_opt module=${GO_PACKAGE_PREFIX}
            ${PROTOS}
            DEPENDS ${FULL_PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "[Gateway][Go] Generating go gateway sources for ${TARGET_NAME}"
            VERBATIM
    )

    add_custom_command(
            OUTPUT ${GO_SW_SOURCE_OUTPUTS}
            COMMAND protobuf::protoc
            ARGS
            ${PROTO_INCLUDE_STRINGS}
            --openapiv2_out ${CMAKE_CURRENT_BINARY_DIR}
            ${PROTOS}
            DEPENDS ${FULL_PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "[Gateway][Go] Generating openapi annotations for ${TARGET_NAME}"
            VERBATIM
    )

    add_custom_target(${TARGET_NAME}-generate-gw-impl
            DEPENDS ${GO_GW_SOURCE_OUTPUTS} ${GO_SW_SOURCE_OUTPUTS})

    string(REGEX REPLACE "(.*)/.+" "\\1" GO_PACKAGE_NAME ${GO_SUBPACKAGE_NAME})

    include(go_utils)
    define_go_executable_target(
            TARGET_NAME ${TARGET_NAME}
            DEPENDS ${DEPENDS}
            REQUIRES
            "github.com/grpc-ecosystem/grpc-gateway/v2 v2.6.0"
            "github.com/grpc-ecosystem/grpc-gateway v1.16.0"
            "google.golang.org/grpc v1.42.0"
            "google.golang.org/genproto v0.0.0-20211118181313-81c1377c94b1"
            GO_PACKAGE_NAME ${GO_PACKAGE_NAME}
            GO_MOD_PATH ${CMAKE_CURRENT_BINARY_DIR}
            RPM_COMPONENT_NAME ${RPM_COMPONENT_NAME}
    )

    add_dependencies(${TARGET_NAME} ${DEPENDS} ${TARGET_NAME}-generate-gw-impl)
endfunction()
