#pragma once

#include <string>

namespace assfire::util
{
    template<typename T>
    struct AmqpProtoEncode
    {
        std::string operator()(const T &val) const {
            return val.SerializeAsString();
        }
    };
}