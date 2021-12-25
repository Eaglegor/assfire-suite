#pragma once

#include <optional>
#include <functional>
#include <amqp.h>
#include "AmqpQueueBinding.hpp"
#include "AmqpQueueOpts.hpp"
#include "AmqpExchangeOpts.hpp"
#include "AmqpError.hpp"
#include "AmqpEnvelopeOpts.hpp"
#include "AmqpDelivery.hpp"
#include "AmqpSubscriptionOpts.hpp"

namespace assfire::util
{
    class AmqpChannel
    {
    public:
        using MessageCallback = std::function<void(AmqpDelivery &)>;

        AmqpChannel(amqp_connection_state_t connection, int channel_id);

        int getId() const;

        void declareExchange(const AmqpExchangeOpts &exchange_opts) const;

        std::string declareQueue(const AmqpQueueOpts &queue_opts) const;

        void deleteQueue(const std::string& queue, bool if_unused, bool if_empty) const;

        void bindQueue(const AmqpQueueBinding &queue_binding) const;

        void publish(const std::string &bytes, const AmqpEnvelopeOpts &options) const;

        std::string subscribe(const AmqpSubscriptionOpts &options) const;
        std::string subscribe(const std::string& consumer_id, const AmqpSubscriptionOpts &options) const;

        void consumeMessage(MessageCallback callback) const;
        void consumeMessage(MessageCallback callback, std::chrono::milliseconds timeout) const;

        void unsubscribe(const std::string &consumer_id) const;

    private:
        amqp_connection_state_t connection;
        int channel_id;
    };
}