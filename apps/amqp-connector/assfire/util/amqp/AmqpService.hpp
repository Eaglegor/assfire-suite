#pragma once

#include <string>
#include "AmqpExchangeOpts.hpp"
#include "AmqpQueueBinding.hpp"
#include "AmqpQueueOpts.hpp"
#include "AmqpConsumerOpts.hpp"
#include "AmqpPublisherOpts.hpp"
#include "AmqpConsumer.hpp"
#include "AmqpPublisher.hpp"

namespace assfire::util {
    class AmqpService {
    public:
        bool declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts);

        bool declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts);

        bool bindQueue(const AmqpQueueBinding &queue_binding);

    private:
    };
}