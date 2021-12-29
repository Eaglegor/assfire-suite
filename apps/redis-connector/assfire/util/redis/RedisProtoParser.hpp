#pragma once

#include <cpp_redis/cpp_redis>
#include "RedisError.hpp"

namespace assfire::util
{
    template<typename T>
    struct RedisProtoParser
    {
        T operator()(const cpp_redis::reply &rpl) {
            if (!rpl.is_string()) {
                throw redis_exception(RedisError(RedisErrorType::INVALID_FORMAT, "couldn't parse reply - expected string"));
            }

            T result;
            result.ParseFromString(rpl.as_string());

            return result;
        }
    };
}