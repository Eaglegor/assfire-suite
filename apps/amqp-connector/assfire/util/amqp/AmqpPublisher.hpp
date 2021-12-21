#pragma once

#include "AmqpPublisherOpts.hpp"

namespace assfire::util {
    class AmqpPublisher {
    public:
        AmqpPublisher(const AmqpPublisherOpts &options) : options(options) {}

        void publish(const char* bytes) {

        }

    private:
        AmqpPublisherOpts options;
    };
}