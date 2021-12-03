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

    set(OUT_SUFFIX proto/cpp)
    make_directory(${CMAKE_BINARY_DIR}/${OUT_SUFFIX})

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
        get_filename_component(proto_filename ${full_proto_path} NAME)
        string(REPLACE ".proto" ".pb.go" proto_go ${proto_filename})
        list(APPEND GO_SOURCE_OUTPUTS "${GO_MOD_PATH}/${proto_go}")
        unset(full_proto_path CACHE)
    endforeach()

    foreach(proto ${GRPC_PROTOS})
        find_file(full_proto_path ${proto} PATHS ${FINAL_IMPORT_DIRS} REQUIRED)
        list(APPEND FULL_GRPC_PROTOS "${full_proto_path}")
        get_filename_component(proto_filename ${full_proto_path} NAME)
        string(REPLACE ".proto" "_grpc.pb.go" proto_grpc_filename ${proto_filename})
        list(APPEND GO_GRPC_SOURCE_OUTPUTS "${GO_MOD_PATH}/${proto_grpc_filename}")
        string(REPLACE ".proto" ".pb.go" proto_grpc_go_filename ${proto_filename})
        list(APPEND GO_SOURCE_OUTPUTS "${GO_MOD_PATH}/${proto_grpc_go_filename}")
        unset(full_proto_path CACHE)
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
            COMMAND ${GO_EXECUTABLE} install
            COMMAND ${GO_EXECUTABLE} build
            WORKING_DIRECTORY ${GO_MOD_PATH}
            DEPENDS ${DEPENDS} ${GO_GRPC_SOURCE_OUTPUTS} ${GO_SOURCE_OUTPUTS} ${GO_MOD_PATH}/go.sum ${GO_MOD_PATH}/go.mod)

    set_target_properties(${TARGET_NAME} PROPERTIES PROTO_IMPORT_DIRS "${FINAL_IMPORT_DIRS}")
    set_target_properties(${TARGET_NAME} PROPERTIES GO_MODULE_PATH "${GO_MOD_PATH}")
    set_target_properties(${TARGET_NAME} PROPERTIES GO_PACKAGE_NAME "${GO_PACKAGE_NAME}")
    set_target_properties(${TARGET_NAME} PROPERTIES DEPENDS_ON "${DEPENDS}")
    set_target_properties(${TARGET_NAME} PROPERTIES TRANSITIVE_DEPS "${TRANSITIVE_DEPS}")
endfunction()