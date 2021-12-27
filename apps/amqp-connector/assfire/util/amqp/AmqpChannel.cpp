#include "AmqpChannel.hpp"
#include <chrono>
#include <spdlog/spdlog.h>
#include <crossguid/guid.hpp>
#include <utility>

#if WIN32

#include <WinSock2.h>

#else
#include <sys/time.h>
#endif

using namespace std::literals::chrono_literals;

namespace assfire::util
{
    AmqpChannel::AmqpChannel()
            : connection(nullptr),
              channel_id(-1) {
    }

    AmqpChannel::AmqpChannel(amqp_connection_state_t connection, int channel_id)
            : connection(connection),
              channel_id(channel_id) {
    }

    void AmqpChannel::declareExchange(const AmqpExchangeOpts &exchange_opts) const {
        SPDLOG_INFO("Declaring exchange {}", exchange_opts.name);
        amqp_exchange_declare(
                connection,
                channel_id,
                exchange_opts.bytesName(),
                exchange_opts.bytesType(),
                exchange_opts.intPassive(),
                exchange_opts.intDurable(),
                exchange_opts.intAutoDelete(),
                exchange_opts.intInternal(),
                exchange_opts.amqpArgs()
        );

        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            throw amqp_exception(AmqpError::fromReply(rpl));
        }
    }

    std::string AmqpChannel::declareQueue(const AmqpQueueOpts &queue_opts) const {
        SPDLOG_INFO("Declaring queue {}", queue_opts.name.empty() ? "<auto_name>" : queue_opts.name);
        auto result = amqp_queue_declare(
                connection,
                channel_id,
                queue_opts.bytesName(),
                queue_opts.intPassive(),
                queue_opts.intDurable(),
                queue_opts.intExclusive(),
                queue_opts.intAutoDelete(),
                queue_opts.amqpArgs()
        );

        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            throw amqp_exception(AmqpError::fromReply(rpl));
        }
        if (queue_opts.name.empty()) {
            SPDLOG_INFO("<auto_name> is resolved to {}", static_cast<const char *>(result->queue.bytes));
        }
        return static_cast<const char *>(result->queue.bytes);
    }

    void AmqpChannel::bindQueue(const AmqpQueueBinding &queue_binding) const {
        SPDLOG_INFO("Binding queue {} to exchange {}", queue_binding.queue_name, queue_binding.exchange_name);
        amqp_queue_bind(
                connection,
                channel_id,
                queue_binding.bytesQueueName(),
                queue_binding.bytesExchangeName(),
                queue_binding.bytesRoutingKey(),
                queue_binding.amqpArgs()
        );

        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            throw amqp_exception(AmqpError::fromReply(rpl));
        }
    }

    void AmqpChannel::publish(const std::string &bytes, const AmqpEnvelopeOpts &options) const {
        auto props = options.amqpProperties();
        amqp_bytes_t message_bytes;
        message_bytes.len = bytes.length();
        message_bytes.bytes = const_cast<void *>(static_cast<const void *>(bytes.c_str()));
        auto result = amqp_basic_publish(
                connection,
                channel_id,
                options.bytesExchangeName(),
                options.bytesRoutingKey(),
                options.mandatory,
                options.immediate,
                props.get(),
                message_bytes
        );

        if (result != AMQP_STATUS_OK) {
            if (result == AMQP_STATUS_CONNECTION_CLOSED ||
                result == AMQP_STATUS_SSL_ERROR ||
                result == AMQP_STATUS_SOCKET_ERROR ||
                result == AMQP_STATUS_TCP_ERROR) {
                throw amqp_exception(AmqpError(AmqpErrorType::CONNECTION_CLOSED, amqp_error_string2(result)));
            }
            throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, amqp_error_string2(result)));
        }
    }

    std::string AmqpChannel::subscribe(const AmqpSubscriptionOpts &options) const {
        std::string consumer_id = xg::newGuid().str();
        return subscribe(consumer_id, options);
    }

    std::string AmqpChannel::subscribe(const std::string &consumer_id, const AmqpSubscriptionOpts &options) const {
        SPDLOG_INFO("Subscribing consumer {} to queue {}", consumer_id, options.queue_name);
        auto result = amqp_basic_consume(
                connection,
                channel_id,
                options.bytesQueueName(),
                amqp_cstring_bytes(consumer_id.c_str()),
                options.intNoLocal(),
                options.inNoAck(),
                options.inExclusive(),
                options.amqpArgs()
        );

        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            throw amqp_exception(AmqpError::fromReply(rpl));
        }

        SPDLOG_INFO("Successfully subscribed consumer {} to queue {} with consumer tag = {}", consumer_id, options.queue_name, static_cast<const char *>(result->consumer_tag.bytes));

        return {
                static_cast<const char *>(result->consumer_tag.bytes),
                result->consumer_tag.len
        };
    }

    void AmqpChannel::consumeMessage(AmqpChannel::MessageCallback callback) const {
        consumeMessage(std::move(callback), 0ms);
    }

    void AmqpChannel::consumeMessage(AmqpChannel::MessageCallback callback, std::chrono::milliseconds timeout) const {
        amqp_envelope_t envelope;

        timeval *timeout_value_ptr = nullptr;
        timeval timeout_value{};
        if (timeout != 0ms) {
            auto sec = std::chrono::duration_cast<std::chrono::seconds>(timeout);
            timeout_value.tv_sec = sec.count();
            timeout_value.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(timeout - sec).count();
            timeout_value_ptr = &timeout_value;
        }

        auto rpl = amqp_consume_message(
                connection,
                &envelope,
                timeout_value_ptr,
                0
        );

        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            throw amqp_exception(AmqpError::fromReply(rpl));
        }

        AmqpDelivery delivery(envelope.message.body.bytes, envelope.message.body.len);

        callback(delivery);

        switch (delivery.getAckStatus()) {
            case AmqpDelivery::AckStatus::NACK: {
                amqp_basic_nack(
                        connection,
                        channel_id,
                        envelope.delivery_tag,
                        false,
                        delivery.intShouldRequeue()
                );

                rpl = amqp_get_rpc_reply(connection);
                if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
                    throw amqp_exception(AmqpError::fromReply(rpl));
                }

                break;
            }
            case AmqpDelivery::AckStatus::ACK: {
                amqp_basic_ack(
                        connection,
                        channel_id,
                        envelope.delivery_tag,
                        false
                );

                rpl = amqp_get_rpc_reply(connection);
                if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
                    throw amqp_exception(AmqpError::fromReply(rpl));
                };

                break;
            }
            default: {
                break;
            }
        }

        amqp_destroy_envelope(&envelope);
    }

    void AmqpChannel::unsubscribe(const std::string &consumer_id) const {
        SPDLOG_INFO("Unsubscribing consumer {}", consumer_id);
        amqp_basic_cancel(connection, channel_id, amqp_cstring_bytes(consumer_id.c_str()));
        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            throw amqp_exception(AmqpError::fromReply(rpl));
        }
    }

    int AmqpChannel::getId() const {
        return channel_id;
    }

    void AmqpChannel::deleteQueue(const std::string &queue, bool if_unused, bool if_empty) const {
        SPDLOG_INFO("Deleting queue {}", queue);
        amqp_queue_delete(
                connection,
                channel_id,
                amqp_cstring_bytes(queue.c_str()),
                if_unused ? 1 : 0,
                if_empty ? 1 : 0
        );

        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            throw amqp_exception(AmqpError::fromReply(rpl));
        }
    }
}