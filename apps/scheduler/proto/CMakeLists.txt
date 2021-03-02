include(proto_utils)

define_proto_target(
        TARGET_NAME assfire-scheduler-proto
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS assfire-concepts-proto assfire-router-proto
        PROTOS assfire/api/v1/scheduler/concepts.proto
)

define_grpc_target(
        TARGET_NAME assfire-scheduler-grpc
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS assfire-scheduler-proto assfire-google-annotations
        PROTOS assfire/api/v1/scheduler/service.proto
)

define_proto_cpp_api_translation_target(
        TARGET_NAME assfire-scheduler-proto-cpp-translation
        PROTO_TARGET assfire-scheduler-proto
        API_TARGET assfire-scheduler-api
        DEPENDS assfire-concepts-proto-cpp-translation assfire-router-proto-cpp-translation
        HEADERS
        assfire/api/v1/scheduler/translators/Translators.hpp
        assfire/api/v1/scheduler/translators/WaybillTranslator.hpp
        assfire/api/v1/scheduler/translators/WaybillAllocationTranslator.hpp
        assfire/api/v1/scheduler/translators/WaybillSchedulingAlgorithmTypeTranslator.hpp
        assfire/api/v1/scheduler/translators/WaybillSchedulerSettingsTranslator.hpp
        assfire/api/v1/scheduler/translators/TimeWindowTranslator.hpp
        SOURCES
        assfire/api/v1/scheduler/translators/WaybillTranslator.cpp
        assfire/api/v1/scheduler/translators/WaybillAllocationTranslator.cpp
        assfire/api/v1/scheduler/translators/WaybillSchedulingAlgorithmTypeTranslator.cpp
        assfire/api/v1/scheduler/translators/WaybillSchedulerSettingsTranslator.cpp
        assfire/api/v1/scheduler/translators/TimeWindowTranslator.cpp
        PUBLIC_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
)