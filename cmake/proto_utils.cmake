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
