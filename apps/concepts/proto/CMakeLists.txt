include(proto_utils)

define_proto_target(
        TARGET_NAME assfire-concepts-proto
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ""
        PROTOS assfire/api/v1/concepts/concepts.proto
)

define_proto_cpp_api_translation_target(
        TARGET_NAME assfire-concepts-proto-cpp-translation
        PROTO_TARGET assfire-concepts-proto
        API_TARGET assfire-concepts-api
        HEADERS
        assfire/api/v1/concepts/translators/Translators.hpp
        assfire/api/v1/concepts/translators/TimePointTranslator.hpp
        assfire/api/v1/concepts/translators/TimeIntervalTranslator.hpp
        assfire/api/v1/concepts/translators/SpeedTranslator.hpp
        assfire/api/v1/concepts/translators/DistanceTranslator.hpp
        SOURCES
        assfire/api/v1/concepts/translators/TimePointTranslator.cpp
        assfire/api/v1/concepts/translators/TimeIntervalTranslator.cpp
        assfire/api/v1/concepts/translators/SpeedTranslator.cpp
        assfire/api/v1/concepts/translators/DistanceTranslator.cpp
        PUBLIC_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
)