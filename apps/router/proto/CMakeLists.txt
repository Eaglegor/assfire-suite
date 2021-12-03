include(proto_utils)

define_proto_target(
        TARGET_NAME assfire-router-proto
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS
        assfire-concepts-proto
        assfire-locations-proto
        PROTOS assfire/api/v1/router/concepts.proto
)

define_grpc_target(
        TARGET_NAME assfire-router-grpc
        PROTO_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS
        assfire-router-proto
        assfire-google-annotations
        PROTOS assfire/api/v1/router/service.proto
)

define_proto_cpp_api_translation_target(
        TARGET_NAME assfire-router-proto-cpp-translation
        PROTO_TARGET assfire-router-proto
        API_TARGET assfire-router-api
        DEPENDS
        assfire-concepts-proto-cpp-translation
        assfire-locations-proto-cpp-translation
        HEADERS
        assfire/api/v1/router/translators/Translators.hpp
        assfire/api/v1/router/translators/RouteInfoTranslator.hpp
        assfire/api/v1/router/translators/RouterEngineTypeTranslator.hpp
        assfire/api/v1/router/translators/RouteProviderSettingsTranslator.hpp
        assfire/api/v1/router/translators/RoutingProfileTranslator.hpp
        SOURCES
        assfire/api/v1/router/translators/RouteInfoTranslator.cpp
        assfire/api/v1/router/translators/RouterEngineTypeTranslator.cpp
        assfire/api/v1/router/translators/RouteProviderSettingsTranslator.cpp
        assfire/api/v1/router/translators/RoutingProfileTranslator.cpp
        PUBLIC_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
)