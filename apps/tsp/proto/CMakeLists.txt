include(proto_utils)

define_proto_target(
        TARGET_NAME assfire-tsp-proto
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS
        assfire-concepts-proto
        assfire-router-proto
        assfire-scheduler-proto
        assfire-locations-proto
        PROTOS
        assfire/api/v1/tsp/concepts.proto
        assfire/api/v1/tsp/worker.proto
        assfire/api/v1/tsp/state.proto
)

define_grpc_target(
        TARGET_NAME assfire-tsp-grpc
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS assfire-tsp-proto assfire-google-annotations
        PROTOS assfire/api/v1/tsp/service.proto
)

define_proto_cpp_api_translation_target(
        TARGET_NAME assfire-tsp-proto-cpp-translation
        PROTO_TARGET assfire-tsp-proto
        API_TARGET assfire-tsp-api
        DEPENDS
        assfire-concepts-proto-cpp-translation
        assfire-router-proto-cpp-translation
        assfire-scheduler-proto-cpp-translation
        assfire-locations-proto-cpp-translation
        HEADERS
        assfire/api/v1/tsp/translators/Translators.hpp
        assfire/api/v1/tsp/translators/TspCostFunctionSettingsTranslator.hpp
        assfire/api/v1/tsp/translators/TspEstimatorSettingsTranslator.hpp
        assfire/api/v1/tsp/translators/TspPointTranslator.hpp
        assfire/api/v1/tsp/translators/TspSolutionTranslator.hpp
        assfire/api/v1/tsp/translators/TspSolverSettingsTranslator.hpp
        assfire/api/v1/tsp/translators/TspTaskTranslator.hpp
        assfire/api/v1/tsp/translators/TspValidationResultTranslator.hpp
        assfire/api/v1/tsp/translators/TspValidatorSettingsTranslator.hpp
        assfire/api/v1/tsp/translators/TspRoutingSettingsTranslator.hpp
        assfire/api/v1/tsp/translators/TspAlgorithmSettingsTranslator.hpp
        assfire/api/v1/tsp/translators/TspAlgorithmTypeTranslator.hpp
        assfire/api/v1/tsp/translators/TspCostTranslator.hpp
        SOURCES
        assfire/api/v1/tsp/translators/TspCostFunctionSettingsTranslator.cpp
        assfire/api/v1/tsp/translators/TspEstimatorSettingsTranslator.cpp
        assfire/api/v1/tsp/translators/TspPointTranslator.cpp
        assfire/api/v1/tsp/translators/TspSolutionTranslator.cpp
        assfire/api/v1/tsp/translators/TspSolverSettingsTranslator.cpp
        assfire/api/v1/tsp/translators/TspTaskTranslator.cpp
        assfire/api/v1/tsp/translators/TspValidationResultTranslator.cpp
        assfire/api/v1/tsp/translators/TspValidatorSettingsTranslator.cpp
        assfire/api/v1/tsp/translators/TspRoutingSettingsTranslator.cpp
        assfire/api/v1/tsp/translators/TspAlgorithmSettingsTranslator.cpp
        assfire/api/v1/tsp/translators/TspAlgorithmTypeTranslator.cpp
        assfire/api/v1/tsp/translators/TspCostTranslator.cpp
        PUBLIC_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
)