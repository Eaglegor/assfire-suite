#pragma once

#include <amqp.h>
#include "AmqpChannel.hpp"
#include <queue>
#include <atomic>
#include <spdlog/spdlog.h>

namespace assfire::util
{
    class AmqpChannelPool
    {
    public:
        AmqpChannelPool()
                : connection(nullptr), channel_counter(0) {}

        AmqpChannel recreateChannel(const AmqpChannel &channel) {
            openChannel(channel.getId());
            return channel;
        }

        AmqpChannel takeChannel() {
            if (channels.empty()) {
                int channel_id = channel_counter++;
                openChannel(channel_id);
                return {connection, channel_id};
            }

            return AmqpChannel(connection, -1);
        }

        void recycleChannel(AmqpChannel channel) {
            channels.push(std::move(channel));
        }

        void replaceConnection(amqp_connection_state_t new_connection) {
            this->connection = new_connection;
            // We only have unconsumed channels in queue
            // When connection is restored we don't need to recreate channels not in use by anyone
            std::queue<AmqpChannel> new_channels;
            channels.swap(new_channels);
        }

        virtual ~AmqpChannelPool() {
            while(!channels.empty()) {
                AmqpChannel c = channels.front();
                channels.pop();
                try {
                    closeChannel(c.getId());
                } catch (const amqp_exception& e) {
                    SPDLOG_WARN("Failed to close AMQP channel {}", c.getId());
                }
            }
        }

    private:
        void openChannel(int channel_id) {
            amqp_channel_open(connection, channel_id);
            amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                throw amqp_exception(AmqpError::fromReply(reply));
            }
        }

        void closeChannel(int channel_id) {
            auto rpl = amqp_channel_close(connection, channel_id, AMQP_REPLY_SUCCESS);
            if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
                throw amqp_exception(AmqpError::fromReply(rpl));
            }
        }

        amqp_connection_state_t connection;
        std::queue<AmqpChannel> channels;
        std::atomic_int channel_counter;
    };
}