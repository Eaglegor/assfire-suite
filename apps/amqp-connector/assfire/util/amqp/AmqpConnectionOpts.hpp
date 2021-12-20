#pragma once

#include <string>

namespace assfire::util {
    struct AmqpConnectionOpts {
        std::string host = "localhost";
        int port = 5672;
        std::string login = "guest";
        std::string password = "guest";

        AmqpConnectionOpts withHost(const std::string& newHost) {
            AmqpConnectionOpts result(*this);
            result.host = newHost;
            return result;
        }

        AmqpConnectionOpts withPort(int newPort) {
            AmqpConnectionOpts result(*this);
            result.port = newPort;
            return result;
        }

        AmqpConnectionOpts withLogin(const std::string& newLogin) {
            AmqpConnectionOpts result(*this);
            result.login = newLogin;
            return result;
        }

        AmqpConnectionOpts withPassword(const std::string& newPassword) {
            AmqpConnectionOpts result(*this);
            result.password = newPassword;
            return result;
        }
    };
}