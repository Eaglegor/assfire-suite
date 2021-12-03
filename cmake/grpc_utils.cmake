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