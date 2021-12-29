set(HEADERS
        assfire/util/redis/RedisConnector.hpp
        assfire/util/redis/RedisError.hpp
        assfire/util/redis/RedisConnectionOpts.hpp
        assfire/util/redis/RedisRetryPolicy.hpp
        assfire/util/redis/RedisProtoParser.hpp
        assfire/util/redis/RedisProtoSerializer.hpp
        )

set(SOURCES
        assfire/util/redis/RedisConnector.cpp
        )

find_package(crossguid CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
include(find_utils)
find_cpp_redis_vcpkg()

add_library(assfire-redis-connector ${SOURCES} ${HEADERS})

target_link_libraries(assfire-redis-connector PRIVATE crossguid cpp_redis::cpp_redis spdlog::spdlog)
target_include_directories(assfire-redis-connector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})