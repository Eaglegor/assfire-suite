include(proto_utils)

define_proto_target(
        TARGET_NAME assfire-locations-proto
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ""
        PROTOS assfire/api/v1/locations/concepts.proto
)

define_proto_cpp_api_translation_target(
        TARGET_NAME assfire-locations-proto-cpp-translation
        PROTO_TARGET assfire-locations-proto
        API_TARGET assfire-locations-api
        HEADERS
        assfire/api/v1/locations/translators/Translators.hpp
        assfire/api/v1/locations/translators/LocationTranslator.hpp
        SOURCES
        assfire/api/v1/locations/translators/LocationTranslator.cpp
        PUBLIC_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
)