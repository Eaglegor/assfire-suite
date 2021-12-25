#pragma once

#include <amqp.h>
#include "AmqpSubscriptionOpts.hpp"

namespace assfire::util {
    struct AmqpConsumerOpts {
        AmqpSubscriptionOpts subscription_opts;
        bool release_queue;
    };
}