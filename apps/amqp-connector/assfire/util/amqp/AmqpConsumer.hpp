#pragma once

#include <functional>
#include "AmqpConsumerOpts.hpp"

namespace assfire::util {
    class AmqpConsumer {
    public:
        using MessageProcessor = std::function<bool>(const char *);

        AmqpConsumer(const AmqpConsumerOpts &options) : options(options) {}

        void consume(MessageProcessor process) {

        }

    private:
        AmqpConsumerOpts options;
    };
}