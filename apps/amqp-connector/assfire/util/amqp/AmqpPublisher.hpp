#pragma once

#include <functional>
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

        void publishBytes(const std::string &bytes) {
            connection.publish(bytes, options.envelope_options);
        }

        template<typename T>
        void publish(const T &obj, std::function<std::string(const T &obj)> encode) {
            publishBytes(encode(obj));
        }

    private:
        std::string name;
        AmqpPublisherOpts options;
        AmqpConnection &connection;
    };
}