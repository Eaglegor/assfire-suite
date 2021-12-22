#pragma once

#include <optional>
#include <amqp.h>
#include "AmqpQueueBinding.hpp"
#include "AmqpQueueOpts.hpp"
#include "AmqpExchangeOpts.hpp"
#include "AmqpError.hpp"

namespace assfire::util {
    class AmqpChannel {
    public:
        AmqpChannel(amqp_connection_state_t connection, int channel_id);

        virtual ~AmqpChannel();

        void declareExchange(const AmqpExchangeOpts &exchange_opts);
        std::string declareQueue(const AmqpQueueOpts &queue_opts);
        void bindQueue(const AmqpQueueBinding &queue_binding);

    private:
        amqp_connection_state_t connection;
        int channel_id;
    };
}