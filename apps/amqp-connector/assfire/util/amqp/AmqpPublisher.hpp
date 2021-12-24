#pragma once

#include <utility>

#include "AmqpPublisherOpts.hpp"
#include "AmqpConnection.hpp"

namespace assfire::util
{
    class AmqpPublisher
    {
    public:
        AmqpPublisher(const std::string &name, AmqpPublisherOpts options, AmqpConnection &connection)
                : options(std::move(options)), connection(connection) {}

        void publish(const std::string &bytes) {
            connection.publish(bytes, options.envelope_options);
        }

    private:
        std::string name;
        AmqpPublisherOpts options;
        AmqpConnection &connection;
    };
}