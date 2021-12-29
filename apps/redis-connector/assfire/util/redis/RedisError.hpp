#pragma once

#include <stdexcept>
#include <string>

namespace assfire::util
{
    enum class RedisErrorType
    {
        UNKNOWN,
        INVALID_FORMAT
    };

    class RedisError
    {
    public:
        RedisError(RedisErrorType error_type, std::string message)
                : error_type(error_type),
                  message(std::move(message)) {

        }

        const RedisErrorType &getType() {
            return error_type;
        }

        const std::string &getMessage() {
            return message;
        }

        static RedisError fromReply(const cpp_redis::reply &rpl) {
            return RedisError(RedisErrorType::UNKNOWN, rpl.error());
        }

        bool isRecoverable() const {
            return true;
        }

    private:
        RedisErrorType error_type;
        std::string message;
    };

    class redis_exception : public std::runtime_error
    {
    public:
        redis_exception(RedisError error)
                : std::runtime_error(error.getMessage()),
                  error(std::move(error)) {}

        const RedisError &getError() const {
            return error;
        }

    private:
        RedisError error;
    };
}
