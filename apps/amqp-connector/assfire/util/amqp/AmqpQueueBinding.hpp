#pragma once

#include <string>
#include <amqp.h>

namespace assfire::util {
    struct AmqpQueueBinding {
        std::string queue_name;
        std::string exchange_name;
        std::string routing_key;

        amqp_bytes_t bytesQueueName() const {
            return amqp_cstring_bytes(queue_name.c_str());
        }

        amqp_bytes_t bytesExchangeName() const {
            return amqp_cstring_bytes(exchange_name.c_str());
        }

        amqp_bytes_t bytesRoutingKey() const {
            return amqp_cstring_bytes(routing_key.c_str());
        }

        amqp_table_t amqpArgs() const {
            return amqp_empty_table;
        }
    };
};