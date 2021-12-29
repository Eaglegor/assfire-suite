#pragma once

#include <string>

namespace assfire::util
{
    template<typename T>
    struct RedisProtoSerializer
    {
        std::string operator()(const T &value) {
            return value.SerializeAsString();
        }
    };
}