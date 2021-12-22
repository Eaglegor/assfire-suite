#pragma once

#include <amqp.h>
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace assfire::util {
    enum class AmqpErrorType {
        UNKNOWN,
        CONNECTION_CLOSED,
        CHANNEL_CLOSED
    };

    struct AmqpError {
        AmqpError(AmqpErrorType type, const std::string &message) : type(type), message(message) {}

        static AmqpError fromReply(const amqp_rpc_reply_t &reply) {
            switch (reply.reply_type) {
                case AMQP_RESPONSE_NONE: {
                    SPDLOG_DEBUG("Missing AMQP reply type");
                    return AmqpError(AmqpErrorType::UNKNOWN, "Missing AMQP reply type");
                }

                case AMQP_RESPONSE_LIBRARY_EXCEPTION: {
                    SPDLOG_DEBUG("Internal AMQP library exception: {}", amqp_error_string2(reply.library_error));
                    return AmqpError(AmqpErrorType::UNKNOWN, amqp_error_string2(reply.library_error));
                }

                case AMQP_RESPONSE_SERVER_EXCEPTION: {
                    switch (reply.reply.id) {
                        case AMQP_CONNECTION_CLOSE_METHOD: {
                            auto reply_decoded = static_cast<amqp_connection_close_t *>(reply.reply.decoded);
                            SPDLOG_DEBUG("AMQP server exception (connection closed): error code {} ({})", reply_decoded->reply_code, static_cast<const char *>(reply_decoded->reply_text.bytes));
                            return AmqpError(AmqpErrorType::CONNECTION_CLOSED, static_cast<const char *>(reply_decoded->reply_text.bytes));
                        }
                        case AMQP_CHANNEL_CLOSE_METHOD: {
                            auto reply_decoded = static_cast<amqp_channel_close_t *>(reply.reply.decoded);
                            SPDLOG_DEBUG("AMQP server exception (channel closed): error code {} ({})", reply_decoded->reply_code, static_cast<const char *>(reply_decoded->reply_text.bytes));
                            return AmqpError(AmqpErrorType::CHANNEL_CLOSED, static_cast<const char *>(reply_decoded->reply_text.bytes));
                        }
                        default: {
                            SPDLOG_DEBUG("Unknown AMQP server exception");
                            return AmqpError(AmqpErrorType::UNKNOWN, "Unknown AMQP server exception");
                        }
                    }
                }

                default: {
                    return AmqpError(AmqpErrorType::UNKNOWN, "Unknown error in AMQP reply");
                }
            }
        }

        AmqpErrorType type;
        std::string message;
    };

    class amqp_exception : public std::exception {
    public:
        amqp_exception(const AmqpError &error) :
                exception(error.message.c_str()),
                error(error) {}

        const AmqpError &getError() const {
            return error;
        }

    private:
        AmqpError error;
    };
}