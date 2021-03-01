function(define_proto_target)
    set(_options "")
    set(_singleargs TARGET_NAME)
    set(_multiargs DEPENDS PROTO_IMPORT_DIRS PROTOS)
    cmake_parse_arguments(define_proto_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_proto_target_TARGET_NAME})
    set(DEPENDS ${define_proto_target_DEPENDS})
    set(PROTO_IMPORT_DIRS ${define_proto_target_PROTO_IMPORT_DIRS})
    set(PROTOS ${define_proto_target_PROTOS})

    message(STATUS "[Protobuf] Generating protobuf definition target: ${TARGET_NAME}")
    message(STATUS "[Protobuf]   Depends on: ${DEPENDS}")
    message(STATUS "[Protobuf]   Import dirs: ${PROTO_IMPORT_DIRS}")
    message(STATUS "[Protobuf]   Proto files: ${PROTOS}")

    add_library(${TARGET_NAME})

    find_package(protobuf CONFIG REQUIRED)
    get_target_property(GOOGLE_PROTOBUF_IMPORT_DIR protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

    list(APPEND IMPORTED_IMPORT_DIRS ${GOOGLE_PROTOBUF_IMPORT_DIR})

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_IMPORTED_DIRS ${dep} PROTO_IMPORT_DIRS)
        list(APPEND IMPORTED_IMPORT_DIRS ${${dep}_IMPORTED_DIRS})
        target_link_libraries(${TARGET_NAME} PUBLIC ${dep})
    endforeach()

    message(STATUS "[Protobuf]   Imported proto import dirs: ${IMPORTED_IMPORT_DIRS}")

    set(FINAL_IMPORT_DIRS ${PROTO_IMPORT_DIRS} ${IMPORTED_IMPORT_DIRS})
    list(REMOVE_DUPLICATES FINAL_IMPORT_DIRS)

    message(STATUS "[Protobuf]   Summarized proto import dirs: ${FINAL_IMPORT_DIRS}")

    set(OUT_SUFFIX proto/cpp)

    protobuf_generate(TARGET ${TARGET_NAME}
            LANGUAGE cpp
            IMPORT_DIRS ${FINAL_IMPORT_DIRS}
            PROTOS ${PROTOS}
            PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/${OUT_SUFFIX}
            )

    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_BINARY_DIR}/${OUT_SUFFIX})
    target_link_libraries(${TARGET_NAME} PUBLIC protobuf::libprotobuf)

    set_target_properties(${TARGET_NAME} PROPERTIES PROTO_IMPORT_DIRS "${FINAL_IMPORT_DIRS}")
endfunction()

function(define_grpc_target)
    set(_options "")
    set(_singleargs TARGET_NAME)
    set(_multiargs DEPENDS PROTO_IMPORT_DIRS PROTOS)
    cmake_parse_arguments(define_grpc_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_grpc_target_TARGET_NAME})
    set(DEPENDS ${define_grpc_target_DEPENDS})
    set(PROTO_IMPORT_DIRS ${define_grpc_target_PROTO_IMPORT_DIRS})
    set(PROTOS ${define_grpc_target_PROTOS})

    message(STATUS "[gRPC] Generating gRPC definition target: ${TARGET_NAME}")
    message(STATUS "[gRPC]   Depends on: ${DEPENDS}")
    message(STATUS "[gRPC]   Import dirs: ${PROTO_IMPORT_DIRS}")
    message(STATUS "[gRPC]   Proto files: ${PROTOS}")

    find_package(protobuf CONFIG REQUIRED)
    find_package(gRPC CONFIG REQUIRED)
    get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)

    add_library(${TARGET_NAME})

    find_package(protobuf CONFIG REQUIRED)
    get_target_property(GOOGLE_PROTOBUF_IMPORT_DIR protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

    list(APPEND IMPORTED_IMPORT_DIRS ${GOOGLE_PROTOBUF_IMPORT_DIR})

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_IMPORTED_DIRS ${dep} PROTO_IMPORT_DIRS)
        list(APPEND IMPORTED_IMPORT_DIRS ${${dep}_IMPORTED_DIRS})
        target_link_libraries(${TARGET_NAME} PUBLIC ${dep})
    endforeach()

    message(STATUS "[gRPC]   Imported proto import dirs: ${IMPORTED_IMPORT_DIRS}")

    set(FINAL_IMPORT_DIRS ${PROTO_IMPORT_DIRS} ${IMPORTED_IMPORT_DIRS})
    list(REMOVE_DUPLICATES FINAL_IMPORT_DIRS)

    message(STATUS "[gRPC]   Summarized proto import dirs: ${FINAL_IMPORT_DIRS}")

    set(OUT_SUFFIX proto/cpp)

    protobuf_generate(TARGET ${TARGET_NAME}
            LANGUAGE cpp
            IMPORT_DIRS ${FINAL_IMPORT_DIRS}
            PROTOS ${PROTOS}
            PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/${OUT_SUFFIX})

    protobuf_generate(TARGET ${TARGET_NAME}
            LANGUAGE grpc
            IMPORT_DIRS ${FINAL_IMPORT_DIRS}
            PROTOS ${PROTOS}
            PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/${OUT_SUFFIX}
            PLUGIN protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
            GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc)

    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_BINARY_DIR}/${OUT_SUFFIX})
    target_link_libraries(${TARGET_NAME} PUBLIC gRPC::grpc++ protobuf::libprotobuf)

    set_target_properties(${TARGET_NAME} PROPERTIES PROTO_IMPORT_DIRS "${FINAL_IMPORT_DIRS}")
endfunction()

function(define_proto_cpp_api_translation_target)
    set(_options "")
    set(_singleargs TARGET_NAME API_TARGET PROTO_TARGET)
    set(_multiargs HEADERS SOURCES PUBLIC_INCLUDE_DIRS DEPENDS)
    cmake_parse_arguments(define_proto_cpp_api_translation_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_proto_cpp_api_translation_target_TARGET_NAME})
    set(API_TARGET ${define_proto_cpp_api_translation_target_API_TARGET})
    set(PROTO_TARGET ${define_proto_cpp_api_translation_target_PROTO_TARGET})
    set(SOURCES ${define_proto_cpp_api_translation_target_SOURCES})
    set(HEADERS ${define_proto_cpp_api_translation_target_HEADERS})
    set(PUBLIC_INCLUDE_DIRS ${define_proto_cpp_api_translation_target_PUBLIC_INCLUDE_DIRS})
    set(DEPENDS ${define_proto_cpp_api_translation_target_DEPENDS})

    message(STATUS "[Api] Generating protobuf-cpp translation target: ${TARGET_NAME}")
    message(STATUS "[Api]   Cpp api target: ${API_TARGET}")
    message(STATUS "[Api]   Proto target: ${PROTO_TARGET}")
    message(STATUS "[Api]   Depends on: ${DEPENDS}")
    message(STATUS "[Api]   Headers: ${HEADERS}")
    message(STATUS "[Api]   Sources: ${SOURCES}")
    message(STATUS "[Api]   Public include dirs: ${PUBLIC_INCLUDE_DIRS}")

    add_library(${TARGET_NAME} ${HEADERS} ${SOURCES})

    foreach(dep ${DEPENDS})
        target_link_libraries(${TARGET_NAME} PUBLIC ${dep})
    endforeach()

    target_link_libraries(${TARGET_NAME} PUBLIC ${API_TARGET} ${PROTO_TARGET})
    target_include_directories(${TARGET_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS})
endfunction()

function(generate_protobuf_include_strings out_var includes_list)
    foreach(inc ${includes_list})
        list(APPEND tmp_string "--proto_path=${inc}")
    endforeach()
    set(${out_var} ${tmp_string} PARENT_SCOPE)
endfunction()

function(generate_grpc_gateway_endpoints_source)
    set(_options "")
    set(_singleargs OUT_DIR)
    set(_multiargs SERVICES GW_MODULE)
    cmake_parse_arguments(generate_grpc_gateway_endpoints_source "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(OUT_DIR ${generate_grpc_gateway_endpoints_source_OUT_DIR})
    set(SERVICES ${generate_grpc_gateway_endpoints_source_SERVICES})
    set(ENABLE_CORS ${ASSFIRE_ALLOW_CORS_IN_GRPC_GATEWAY})
    set(GW_MODULE ${generate_grpc_gateway_endpoints_source_GW_MODULE})

    foreach(service ${SERVICES})
        string(APPEND ASSFIRE_REGISTER_GRPC_SERVICE_ENDPOINTS "
  err := gw.Register${service}HandlerFromEndpoint(ctx, mux,  *grpcServerEndpoint, opts)
  if err != nil {
   return err
  }
        ")
    endforeach()

    set(ASSFIRE_GRPC_GATEWAY_MODULE ${GW_MODULE})

    message(DEBUG "Generated endpoints: ${ASSFIRE_REGISTER_GRPC_SERVICE_ENDPOINTS}")

    file(MAKE_DIRECTORY ${OUT_DIR}/assfire.org)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/grpc_gateway/assfire-grpc-proxy.go ${OUT_DIR}/assfire.org @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/grpc_gateway/go.mod ${OUT_DIR}/assfire.org @ONLY)
endfunction()

function(define_go_grpc_proxy_target)
    set(_options "")
    set(_singleargs TARGET_NAME RPM_COMPONENT_NAME GW_MODULE)
    set(_multiargs DEPENDS PROTO_IMPORT_DIRS PROTOS)
    cmake_parse_arguments(define_go_grpc_proxy_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_go_grpc_proxy_target_TARGET_NAME})
    set(DEPENDS ${define_go_grpc_proxy_target_DEPENDS})
    set(PROTO_IMPORT_DIRS ${define_go_grpc_proxy_target_PROTO_IMPORT_DIRS})
    set(PROTOS ${define_go_grpc_proxy_target_PROTOS})
    set(GW_MODULE ${define_go_grpc_proxy_target_GW_MODULE})
    set(RPM_COMPONENT_NAME ${define_go_grpc_proxy_target_RPM_COMPONENT_NAME})

    message(STATUS "[Gateway] Generating grpc-gateway target: ${TARGET_NAME}")
    message(STATUS "[Gateway]   Depends on: ${DEPENDS}")
    message(STATUS "[Gateway]   Additional proto import dirs: ${PROTO_IMPORT_DIRS}")
    message(STATUS "[Gateway]   Protos: ${PROTOS}")

    find_package(protobuf CONFIG REQUIRED)
    get_target_property(GOOGLE_PROTOBUF_IMPORT_DIR protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

    list(APPEND IMPORTED_IMPORT_DIRS ${GOOGLE_PROTOBUF_IMPORT_DIR})

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_IMPORTED_DIRS ${dep} PROTO_IMPORT_DIRS)
        list(APPEND IMPORTED_IMPORT_DIRS ${${dep}_IMPORTED_DIRS})
    endforeach()

    set(FINAL_IMPORT_DIRS ${PROTO_IMPORT_DIRS} ${IMPORTED_IMPORT_DIRS})
    list(REMOVE_DUPLICATES FINAL_IMPORT_DIRS)

    message(STATUS "[Gateway]   Summarized import dirs: ${FINAL_IMPORT_DIRS}")

    generate_protobuf_include_strings(PROTO_INCLUDE_STRINGS "${FINAL_IMPORT_DIRS}")

    foreach(proto ${PROTOS})
        find_file(full_proto_path ${proto} PATHS ${FINAL_IMPORT_DIRS} REQUIRED)
        list(APPEND FULL_PROTOS "${full_proto_path}")
        unset(full_proto_path CACHE)
        string(REPLACE ".proto" ".pb.go" proto_go ${proto})
        list(APPEND GO_SOURCE_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${proto_go}")
        string(REPLACE ".proto" ".pb.gw.go" proto_gw ${proto})
        list(APPEND GO_GW_SOURCE_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${proto_gw}")
        string(REPLACE ".proto" ".json" proto_sw ${proto})
        list(APPEND GO_SW_SOURCE_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${proto_sw}")
    endforeach()

    add_custom_command(
            OUTPUT ${GO_SOURCE_OUTPUTS}
            COMMAND protobuf::protoc
            ARGS
            ${PROTO_INCLUDE_STRINGS}
            --go_out ${CMAKE_CURRENT_BINARY_DIR}
            --go_opt plugins=grpc
            ${PROTOS}
            DEPENDS ${FULL_PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "[Gateway] Generating go sources for ${TARGET_NAME}"
            VERBATIM
    )

    add_custom_command(
            OUTPUT ${GO_GW_SOURCE_OUTPUTS}
            COMMAND protobuf::protoc
            ARGS
            ${PROTO_INCLUDE_STRINGS}
            --grpc-gateway_out ${CMAKE_CURRENT_BINARY_DIR}
            ${PROTOS}
            DEPENDS ${FULL_PROTOS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "[Gateway] Generating go gateway sources for ${TARGET_NAME}"
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
            COMMENT "[Gateway] Generating openapi annotations for ${TARGET_NAME}"
            VERBATIM
    )

    foreach(proto_file ${FULL_PROTOS})
        file(STRINGS ${proto_file} proto_file_contents REGEX ".*service[ ]+[a-zA-Z0-9]+.*")
        foreach(service_string ${proto_file_contents})
            string(REGEX REPLACE ".*service[ ]+([a-zA-Z0-9]+).*" "\\1" service_name ${service_string})
            list(APPEND SERVICE_NAMES ${service_name})
        endforeach()
    endforeach()

    message(DEBUG "Parsed service names: ${SERVICE_NAMES}")

    generate_grpc_gateway_endpoints_source(
            OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
            SERVICES "${SERVICE_NAMES}"
            GW_MODULE "${GW_MODULE}"
    )

    add_custom_target(${TARGET_NAME} ALL
            COMMAND ${GO_EXECUTABLE} get
            COMMAND ${GO_EXECUTABLE} build
            -o ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assfire.org
            DEPENDS ${GO_SOURCE_OUTPUTS} ${GO_GW_SOURCE_OUTPUTS} ${GO_SW_SOURCE_OUTPUTS})

    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX} COMPONENT ${RPM_COMPONENT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    if(ASSFIRE_PACKAGE_BUILD_RPM)
        cpack_add_component(${RPM_COMPONENT_NAME})
    endif()

endfunction()
