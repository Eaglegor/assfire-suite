#pragma once

#include "AmqpChannel.hpp"

namespace assfire::util {
    class ChannelRef {
    public:
        const AmqpChannel &get() const {
            return channel;
        }

        int getId() const {
            return channel.getId();
        }

    private:
        AmqpChannel channel;
    };

    class AmqpChannelPool {
    public:
        void putChannel(AmqpChannel channel);

        ChannelRef takeChannel();
    };
}