#pragma once

#include <amqp.h>
#include <stdexcept>
#include <utility>
#include <spdlog/spdlog.h>

namespace assfire::util
{
    enum class AmqpErrorType
    {
        UNKNOWN,
        CONNECTION_CLOSED,
        CHANNEL_CLOSED,
        TIMEOUT
    };

    struct AmqpError
    {
        AmqpError(AmqpErrorType type, std::string message)
                : type(type), message(std::move(message)) {}

        static AmqpError fromReply(const amqp_rpc_reply_t &reply) {
            switch (reply.reply_type) {
                case AMQP_RESPONSE_NONE: {
                    SPDLOG_DEBUG("Missing AMQP reply type");
                    return {AmqpErrorType::UNKNOWN, "Missing AMQP reply type"};
                }

                case AMQP_RESPONSE_LIBRARY_EXCEPTION: {
                    SPDLOG_DEBUG("Internal AMQP library exception: {}", amqp_error_string2(reply.library_error));
                    switch(reply.library_error) {
                        case AMQP_STATUS_TIMEOUT:
                            return {AmqpErrorType::TIMEOUT, amqp_error_string2(reply.library_error)};
                        case AMQP_STATUS_SOCKET_ERROR:
                        case AMQP_STATUS_TCP_ERROR:
                        case AMQP_STATUS_SSL_ERROR:
                        case AMQP_STATUS_CONNECTION_CLOSED:
                        case AMQP_STATUS_SOCKET_CLOSED:
                        case AMQP_STATUS_SSL_CONNECTION_FAILED:
                            return {AmqpErrorType::CONNECTION_CLOSED, amqp_error_string2(reply.library_error)};
                        default:
                            return {AmqpErrorType::UNKNOWN, amqp_error_string2(reply.library_error)};
                    }
                }

                case AMQP_RESPONSE_SERVER_EXCEPTION: {
                    switch (reply.reply.id) {
                        case AMQP_CONNECTION_CLOSE_METHOD: {
                            auto reply_decoded = static_cast<amqp_connection_close_t *>(reply.reply.decoded);
                            SPDLOG_ERROR("AMQP server exception (connection closed): error code {} ({})", reply_decoded->reply_code, static_cast<const char *>(reply_decoded->reply_text.bytes));
                            return {AmqpErrorType::CONNECTION_CLOSED, static_cast<const char *>(reply_decoded->reply_text.bytes)};
                        }
                        case AMQP_CHANNEL_CLOSE_METHOD: {
                            auto reply_decoded = static_cast<amqp_channel_close_t *>(reply.reply.decoded);
                            SPDLOG_ERROR("AMQP server exception (channel closed): error code {} ({})", reply_decoded->reply_code, static_cast<const char *>(reply_decoded->reply_text.bytes));
                            return {AmqpErrorType::CHANNEL_CLOSED, static_cast<const char *>(reply_decoded->reply_text.bytes)};
                        }
                        default: {
                            SPDLOG_ERROR("Unknown AMQP server exception");
                            return {AmqpErrorType::UNKNOWN, "Unknown AMQP server exception"};
                        }
                    }
                }

                default: {
                    return {AmqpErrorType::UNKNOWN, "Unknown error in AMQP reply"};
                }
            }
        }

        std::string message;
        AmqpErrorType type;
    };

    class amqp_exception : public std::runtime_error
    {
    public:
        explicit amqp_exception(AmqpError error)
                :
                std::runtime_error(error.message),
                error(std::move(error)) {}

        amqp_exception(const amqp_exception &rhs) noexcept = default;

        const AmqpError &getError() const {
            return error;
        }

    private:
        AmqpError error;
    };
}