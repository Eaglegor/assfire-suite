#pragma once

#include <string>
#include <functional>

namespace assfire::util
{
    template<typename T>
    struct RedisProtoSerializer
    {
        std::string operator()(const T &value) {
            return value.SerializeAsString();
        }
    };

    using RedisStringSerializer = std::identity;
}