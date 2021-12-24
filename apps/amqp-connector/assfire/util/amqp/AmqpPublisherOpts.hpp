#pragma once

#include <amqp.h>
#include "AmqpEnvelopeOpts.hpp"

namespace assfire::util {
    struct AmqpPublisherOpts {
        AmqpEnvelopeOpts envelope_options;
    };
}