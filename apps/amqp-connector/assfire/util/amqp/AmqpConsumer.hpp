#pragma once

#include <functional>
#include <chrono>
#include "AmqpConsumerOpts.hpp"
#include "AmqpChannel.hpp"
#include "AmqpConnection.hpp"
#include <spdlog/spdlog.h>

namespace assfire::util
{
    class AmqpConsumer
    {
    public:
        AmqpConsumer(const std::string &name, const AmqpConsumerOpts &options, AmqpConnection &connection)
                : name(name), options(options), connection(connection) {}

        AmqpConsumer(const AmqpConsumer& rhs) = delete;

        void consumeMessage(AmqpChannel::MessageCallback process) {
            if (consumer_id.empty()) subscribe();
            connection.consumeMessage(consumer_id, process);
        }

        void consumeMessage(AmqpChannel::MessageCallback process, std::chrono::milliseconds timeout) {
            if (consumer_id.empty()) subscribe();
            connection.consumeMessage(consumer_id, process, timeout);
        }

        ~AmqpConsumer() {
            if (!consumer_id.empty()) {
                try {
                    unsubscribe();
                    if(options.release_queue) {

                    }
                } catch (const amqp_exception &e) {
                    SPDLOG_ERROR("Failed to unsubscribe AQMP consumer {}, consumer_id = {}: {}", name, consumer_id, e.what());
                }
            }
        }

    private:
        void subscribe() {
            consumer_id = connection.subscribe(options.subscription_opts);
        }

        void unsubscribe() {
            connection.unsubscribe(consumer_id);
        }

        std::string name;
        AmqpConsumerOpts options;
        AmqpConnection &connection;
        std::string consumer_id;
    };
}