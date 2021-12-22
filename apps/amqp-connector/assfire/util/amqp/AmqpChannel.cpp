#include "AmqpChannel.hpp"
#include <spdlog/spdlog.h>

namespace assfire::util {
    AmqpChannel::AmqpChannel(amqp_connection_state_t connection, int channel_id) :
            connection(connection),
            channel_id(channel_id) {

        amqp_channel_open(connection, channel_id);

        auto reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            SPDLOG_ERROR("Failed to open channel {}: {}", channel_id, AmqpError::fromReply(reply).message);
        }
    }

    AmqpChannel::~AmqpChannel() {
        amqp_channel_close(connection, channel_id, AMQP_REPLY_SUCCESS);

        auto reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            SPDLOG_ERROR("Failed to close channel {}, {}", channel_id, AmqpError::fromReply(reply).message);
        }
    }

    void AmqpChannel::declareExchange(const AmqpExchangeOpts &exchange_opts) {
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

    std::string AmqpChannel::declareQueue(const AmqpQueueOpts &queue_opts) {
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
        return static_cast<const char *>(result->queue.bytes);
    }

    void AmqpChannel::bindQueue(const AmqpQueueBinding &queue_binding) {
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
}