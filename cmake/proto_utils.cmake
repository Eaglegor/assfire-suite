function(define_proto_target)
    set(_options "")
    set(_singleargs TARGET_NAME)
    set(_multiargs DEPENDS PROTO_IMPORT_DIRS PROTOS)
    cmake_parse_arguments(define_proto_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_proto_target_TARGET_NAME})
    set(DEPENDS ${define_proto_target_DEPENDS})
    set(PROTO_IMPORT_DIRS ${define_proto_target_PROTO_IMPORT_DIRS})
    set(PROTOS ${define_proto_target_PROTOS})

    message(STATUS "[Protobuf][C++] Generating protobuf definition target: ${TARGET_NAME}")
    message(VERBOSE "[Protobuf][C++]   Depends on: ${DEPENDS}")
    message(VERBOSE "[Protobuf][C++]   Import dirs: ${PROTO_IMPORT_DIRS}")
    message(VERBOSE "[Protobuf][C++]   Proto files: ${PROTOS}")

    add_library(${TARGET_NAME})

    find_package(protobuf CONFIG REQUIRED)
    get_target_property(GOOGLE_PROTOBUF_IMPORT_DIR protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

    list(APPEND IMPORTED_IMPORT_DIRS ${GOOGLE_PROTOBUF_IMPORT_DIR})

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_IMPORTED_DIRS ${dep} PROTO_IMPORT_DIRS)
        list(APPEND IMPORTED_IMPORT_DIRS ${${dep}_IMPORTED_DIRS})
        target_link_libraries(${TARGET_NAME} PUBLIC ${dep})
    endforeach()

    message(VERBOSE "[Protobuf][C++]   Imported proto import dirs: ${IMPORTED_IMPORT_DIRS}")

    set(FINAL_IMPORT_DIRS ${PROTO_IMPORT_DIRS} ${IMPORTED_IMPORT_DIRS})
    list(REMOVE_DUPLICATES FINAL_IMPORT_DIRS)

    message(VERBOSE "[Protobuf][C++]   Summarized proto import dirs: ${FINAL_IMPORT_DIRS}")

    make_directory(${CMAKE_BINARY_DIR}/proto/cpp)

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

function(define_proto_go_target)
    set(_options "")
    set(_singleargs TARGET_NAME GO_PACKAGE_NAME GO_MOD_PATH PROTO_ROOT)
    set(_multiargs DEPENDS PROTO_IMPORT_DIRS PROTOS GRPC_PROTOS REQUIRES)
    cmake_parse_arguments(define_proto_go_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_proto_go_target_TARGET_NAME})
    set(DEPENDS ${define_proto_go_target_DEPENDS})
    set(PROTO_IMPORT_DIRS ${define_proto_go_target_PROTO_IMPORT_DIRS})
    set(PROTOS ${define_proto_go_target_PROTOS})
    set(PROTO_ROOT ${define_proto_go_target_PROTO_ROOT})
    set(GO_PACKAGE_NAME ${define_proto_go_target_GO_PACKAGE_NAME})
    set(REQUIRES ${define_proto_go_target_REQUIRES})
    set(GRPC_PROTOS ${define_proto_go_target_GRPC_PROTOS})

    list(APPEND REQUIRES "google.golang.org/protobuf v1.27.1")

    message(STATUS "[Protobuf][Go] Generating protobuf definition target: ${TARGET_NAME}")
    message(VERBOSE "[Protobuf][Go]   Depends on: ${DEPENDS}")
    message(VERBOSE "[Protobuf][Go]   Import dirs: ${PROTO_IMPORT_DIRS}")
    message(VERBOSE "[Protobuf][Go]   Proto files: ${PROTOS}")
    message(VERBOSE "[Protobuf][Go]   gRPC proto files: ${GRPC_PROTOS}")
    message(VERBOSE "[Protobuf][Go]   Proto root: ${PROTO_ROOT}")
    message(VERBOSE "[Protobuf][Go]   Go package name: ${GO_PACKAGE_NAME}")
    message(VERBOSE "[Protobuf][Go]   Requires: ${REQUIRES}")

    find_package(protobuf CONFIG REQUIRED)
    get_target_property(GOOGLE_PROTOBUF_IMPORT_DIR protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

    list(APPEND IMPORTED_IMPORT_DIRS ${GOOGLE_PROTOBUF_IMPORT_DIR})
    list(APPEND IMPORTED_IMPORT_DIRS "${CMAKE_SOURCE_DIR}/apps/google-annotations")

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_IMPORTED_DIRS ${dep} PROTO_IMPORT_DIRS)
        list(APPEND IMPORTED_IMPORT_DIRS ${${dep}_IMPORTED_DIRS})
    endforeach()

    message(VERBOSE "[Protobuf][Go]   Imported proto import dirs: ${IMPORTED_IMPORT_DIRS}")

    set(FINAL_IMPORT_DIRS ${PROTO_ROOT} ${PROTO_IMPORT_DIRS} ${IMPORTED_IMPORT_DIRS})
    list(REMOVE_DUPLICATES FINAL_IMPORT_DIRS)

    generate_protobuf_include_strings(PROTO_INCLUDE_STRINGS "${FINAL_IMPORT_DIRS}")

    message(VERBOSE "[Protobuf][Go]   Summarized proto import dirs: ${FINAL_IMPORT_DIRS}")
    message(VERBOSE "[Protobuf][Go]   Summarized proto include strings: ${PROTO_INCLUDE_STRINGS}")

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_TRANSITIVE_DEPS ${dep} DEPENDS_ON)
        list(APPEND TRANSITIVE_DEPS ${${dep}_TRANSITIVE_DEPS})
    endforeach()

    list(REMOVE_DUPLICATES TRANSITIVE_DEPS)

    set(GO_MOD_PATH ${CMAKE_CURRENT_BINARY_DIR}/${GO_PACKAGE_NAME})

    foreach(proto ${PROTOS})
        find_file(full_proto_path ${proto} PATHS ${FINAL_IMPORT_DIRS} REQUIRED)
        list(APPEND FULL_PROTOS "${full_proto_path}")
        unset(full_proto_path CACHE)
        string(REPLACE ".proto" ".pb.go" proto_go ${proto})
        list(APPEND GO_SOURCE_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${proto_go}")
    endforeach()

    foreach(proto ${GRPC_PROTOS})
        find_file(full_proto_path ${proto} PATHS ${FINAL_IMPORT_DIRS} REQUIRED)
        list(APPEND FULL_GRPC_PROTOS "${full_proto_path}")
        unset(full_proto_path CACHE)
        string(REPLACE ".proto" "_grpc.pb.go" proto_grpc ${proto})
        list(APPEND GO_GRPC_SOURCE_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${proto_grpc}")
        string(REPLACE ".proto" ".pb.go" proto_grpc_go ${proto})
        list(APPEND GO_SOURCE_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${proto_grpc_go}")
    endforeach()

    message(VERBOSE "[Protobuf][Go]   Expected sources: ${GO_SOURCE_OUTPUTS}")
    message(VERBOSE "[Protobuf][Go]   Expected gRPC sources: ${GO_GRPC_SOURCE_OUTPUTS}")

    file(MAKE_DIRECTORY ${GO_MOD_PATH})

    set(REQUIRE_DIRECTIVES "")
    set(REPLACE_DIRECTIVES "")
    foreach(req ${REQUIRES})
        string(APPEND REQUIRE_DIRECTIVES "${req}\n")
    endforeach()
    foreach(req ${DEPENDS})
        get_target_property(DEP_MODULE_NAME ${req} GO_PACKAGE_NAME)
        get_target_property(DEP_MODULE_PATH ${req} GO_MODULE_PATH)

        string(APPEND REQUIRE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION}\n")
        string(APPEND REPLACE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION} => ${DEP_MODULE_PATH}\n")
    endforeach()
    foreach(req ${TRANSITIVE_DEPS})
        get_target_property(DEP_MODULE_NAME ${req} GO_PACKAGE_NAME)
        get_target_property(DEP_MODULE_PATH ${req} GO_MODULE_PATH)

        string(APPEND REPLACE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION} => ${DEP_MODULE_PATH}\n")
    endforeach()
    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/go.mod ${GO_MOD_PATH} @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/go.sum ${GO_MOD_PATH} @ONLY)

    add_custom_command(
            OUTPUT ${GO_SOURCE_OUTPUTS}
            COMMAND protobuf::protoc
            ARGS
            ${PROTO_INCLUDE_STRINGS}
            --go_out ${CMAKE_CURRENT_BINARY_DIR}
            ${PROTOS} ${GRPC_PROTOS}
            WORKING_DIRECTORY ${PROTO_ROOT}
            DEPENDS ${FULL_PROTOS} ${FULL_GRPC_PROTOS}
            COMMENT "[Protobuf][Go] Generating go sources for ${TARGET_NAME}"
            VERBATIM
    )

    add_custom_command(
            OUTPUT "${GO_GRPC_SOURCE_OUTPUTS}"
            COMMAND protobuf::protoc
            ARGS
            ${PROTO_INCLUDE_STRINGS}
            --go-grpc_out ${CMAKE_CURRENT_BINARY_DIR}
            ${GRPC_PROTOS}
            DEPENDS "${FULL_GRPC_PROTOS}"
            WORKING_DIRECTORY ${PROTO_ROOT}
            COMMENT "[Protobuf][Go] Generating go gRPC sources for ${TARGET_NAME}"
            VERBATIM
    )

    include(find_utils)
    find_required_program(go GO_EXECUTABLE)

    add_custom_target(${TARGET_NAME} ALL
            COMMAND ${GO_EXECUTABLE} get
            COMMAND ${GO_EXECUTABLE} build
            WORKING_DIRECTORY ${GO_MOD_PATH}
            DEPENDS ${GO_GRPC_SOURCE_OUTPUTS} ${GO_SOURCE_OUTPUTS} ${GO_MOD_PATH}/go.sum ${GO_MOD_PATH}/go.mod)

    set_target_properties(${TARGET_NAME} PROPERTIES PROTO_IMPORT_DIRS "${FINAL_IMPORT_DIRS}")
    set_target_properties(${TARGET_NAME} PROPERTIES GO_MODULE_PATH "${GO_MOD_PATH}")
    set_target_properties(${TARGET_NAME} PROPERTIES GO_PACKAGE_NAME "${GO_PACKAGE_NAME}")
    set_target_properties(${TARGET_NAME} PROPERTIES DEPENDS_ON "${DEPENDS}")
    set_target_properties(${TARGET_NAME} PROPERTIES TRANSITIVE_DEPS "${TRANSITIVE_DEPS}")
endfunction()

function(define_go_executable_target)
    set(_options "")
    set(_singleargs TARGET_NAME GO_PACKAGE_NAME GO_MOD_PATH RPM_COMPONENT_NAME)
    set(_multiargs DEPENDS REQUIRES)
    cmake_parse_arguments(define_go_target "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

    set(TARGET_NAME ${define_go_target_TARGET_NAME})
    set(DEPENDS ${define_go_target_DEPENDS})
    set(REQUIRES ${define_go_target_REQUIRES})
    set(GO_PACKAGE_NAME ${define_go_target_GO_PACKAGE_NAME})
    set(GO_MOD_PATH ${define_go_target_GO_MOD_PATH})
    set(RPM_COMPONENT_NAME ${define_go_target_RPM_COMPONENT_NAME})

    foreach(dep ${DEPENDS})
        get_target_property(${dep}_TRANSITIVE_DEPS ${dep} DEPENDS_ON)
        if(NOT ${dep}_TRANSITIVE_DEPS STREQUAL "${dep}_TRANSITIVE_DEPS-NOTFOUND")
            list(APPEND TRANSITIVE_DEPS ${${dep}_TRANSITIVE_DEPS})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES TRANSITIVE_DEPS)

    list(APPEND REQUIRES "google.golang.org/protobuf v1.27.1")

    message(STATUS "[Server][Go] Generating go server target: ${TARGET_NAME}")
    message(VERBOSE "[Server][Go]   Depends on: ${DEPENDS}")
    message(VERBOSE "[Server][Go]   Go package name: ${GO_PACKAGE_NAME}")
    message(VERBOSE "[Server][Go]   Requires: ${REQUIRES}")
    message(VERBOSE "[Server][Go]   Transitive dependencies: ${TRANSITIVE_DEPS}")

    if(NOT GO_MOD_PATH)
        set(GO_MOD_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(REQUIRE_DIRECTIVES "")
    set(REPLACE_DIRECTIVES "")
    foreach(req ${REQUIRES})
        string(APPEND REQUIRE_DIRECTIVES "${req}\n")
    endforeach()
    foreach(req ${DEPENDS})
        get_target_property(DEP_MODULE_NAME ${req} GO_PACKAGE_NAME)
        get_target_property(DEP_MODULE_PATH ${req} GO_MODULE_PATH)

        if(NOT DEP_MODULE_NAME STREQUAL "DEP_MODULE_NAME-NOTFOUND" AND NOT DEP_MODULE_PATH STREQUAL "DEP_MODULE_PATH-NOTFOUND")
            string(APPEND REQUIRE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION}\n")
            string(APPEND REPLACE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION} => ${DEP_MODULE_PATH}\n")
        endif()
    endforeach()
    foreach(req ${TRANSITIVE_DEPS})
        get_target_property(DEP_MODULE_NAME ${req} GO_PACKAGE_NAME)
        get_target_property(DEP_MODULE_PATH ${req} GO_MODULE_PATH)
        if(NOT DEP_MODULE_NAME STREQUAL "DEP_MODULE_NAME-NOTFOUND" AND NOT DEP_MODULE_PATH STREQUAL "DEP_MODULE_PATH-NOTFOUND")
            string(APPEND REPLACE_DIRECTIVES "${DEP_MODULE_NAME} ${ASSFIRE_APPLICATION_RELEASE_VERSION} => ${DEP_MODULE_PATH}\n")
        endif()
    endforeach()
    message(VERBOSE "[Server][Go] Resolved go.mod require directives: ${REQUIRE_DIRECTIVES}")
    message(VERBOSE "[Server][Go] Resolved go.mod replace directives: ${REPLACE_DIRECTIVES}")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/go.mod ${GO_MOD_PATH} @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/go.sum ${GO_MOD_PATH} @ONLY)

    configure_file(${CMAKE_SOURCE_DIR}/cmake/go/dev.gitignore ${CMAKE_CURRENT_SOURCE_DIR}/.gitignore)

    include(find_utils)
    find_required_program(go GO_EXECUTABLE)

    add_custom_target(${TARGET_NAME} ALL
            COMMAND ${GO_EXECUTABLE} get
            COMMAND ${GO_EXECUTABLE} build -o ${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}
            WORKING_DIRECTORY ${GO_MOD_PATH}
            DEPENDS ${GO_MOD_PATH}/go.sum ${GO_MOD_PATH}/go.mod)

    set_target_properties(${TARGET_NAME} PROPERTIES PROTO_IMPORT_DIRS "${FINAL_IMPORT_DIRS}")
    set_target_properties(${TARGET_NAME} PROPERTIES GO_MODULE_PATH "${GO_MOD_PATH}")
    set_target_properties(${TARGET_NAME} PROPERTIES GO_PACKAGE_NAME "${GO_PACKAGE_NAME}")
    set_target_properties(${TARGET_NAME} PROPERTIES DEPENDS_ON "${DEPENDS}")
    set_target_properties(${TARGET_NAME} PROPERTIES TRANSITIVE_DEPS "${TRANSITIVE_DEPS}")

    install(PROGRAMS ${GO_MOD_PATH}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX} TYPE BIN COMPONENT ${RPM_COMPONENT_NAME})
    if(ASSFIRE_RPM_ENABLED)
        cpack_add_component(${RPM_COMPONENT_NAME})
    endif()
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

    message(STATUS "[gRPC][C++] Generating gRPC definition target: ${TARGET_NAME}")
    message(VERBOSE "[gRPC][C++]   Depends on: ${DEPENDS}")
    message(VERBOSE "[gRPC][C++]   Import dirs: ${PROTO_IMPORT_DIRS}")
    message(VERBOSE "[gRPC][C++]   Proto files: ${PROTOS}")

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

    message(VERBOSE "[gRPC][C++]   Imported proto import dirs: ${IMPORTED_IMPORT_DIRS}")

    set(FINAL_IMPORT_DIRS ${PROTO_IMPORT_DIRS} ${IMPORTED_IMPORT_DIRS})
    list(REMOVE_DUPLICATES FINAL_IMPORT_DIRS)

    message(VERBOSE "[gRPC][C++]   Summarized proto import dirs: ${FINAL_IMPORT_DIRS}")

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

    message(STATUS "[Api][C++] Generating protobuf-cpp translation target: ${TARGET_NAME}")
    message(VERBOSE "[Api][C++]   Cpp api target: ${API_TARGET}")
    message(VERBOSE "[Api][C++]   Proto target: ${PROTO_TARGET}")
    message(VERBOSE "[Api][C++]   Depends on: ${DEPENDS}")
    message(VERBOSE "[Api][C++]   Headers: ${HEADERS}")
    message(VERBOSE "[Api][C++]   Sources: ${SOURCES}")
    message(VERBOSE "[Api][C++]   Public include dirs: ${PUBLIC_INCLUDE_DIRS}")

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

    define_go_executable_target(
            TARGET_NAME ${TARGET_NAME}
            DEPENDS ${DEPENDS}
            REQUIRES
            "github.com/grpc-ecosystem/grpc-gateway/v2 v2.6.0"
            "google.golang.org/grpc v1.35.0"
            "google.golang.org/genproto v0.0.0-20210207032614-bba0dbe2a9ea"
            GO_PACKAGE_NAME ${GO_PACKAGE_NAME}
            GO_MOD_PATH ${CMAKE_CURRENT_BINARY_DIR}
            RPM_COMPONENT_NAME ${RPM_COMPONENT_NAME}
    )

    add_dependencies(${TARGET_NAME} ${DEPENDS} ${TARGET_NAME}-generate-gw-impl)
endfunction()
