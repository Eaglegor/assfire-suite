#pragma once

#include <functional>
#include "AmqpConsumerOpts.hpp"
#include "AmqpChannel.hpp"
#include "AmqpConnection.hpp"

namespace assfire::util
{
    class AmqpConsumer
    {
    public:
        AmqpConsumer(const std::string &name, const AmqpConsumerOpts &options, AmqpConnection &connection)
                : name(name), options(options), connection(connection) {}

        void consumeMessage(AmqpChannel::MessageCallback process) {

        }

    private:
        std::string name;
        AmqpConsumerOpts options;
        AmqpConnection &connection;
    };
}