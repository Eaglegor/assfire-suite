#pragma once

#include <string>

namespace assfire::util {
    struct RedisConnectionOpts {
        std::string host;
        int port;
        std::string login;
        std::string password;
    };
}