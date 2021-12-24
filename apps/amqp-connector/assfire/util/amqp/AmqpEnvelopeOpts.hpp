#pragma once

#include <string>
#include <memory>
#include <amqp.h>

namespace assfire::util {
    struct AmqpEnvelopeOpts {
        std::string exchange_name;
        std::string routing_key;
        bool mandatory;
        bool immediate;


        amqp_bytes_t bytesExchangeName() const {
            return amqp_cstring_bytes(exchange_name.c_str());
        }

        amqp_bytes_t bytesRoutingKey() const {
            return amqp_cstring_bytes(routing_key.c_str());
        }

        std::unique_ptr<amqp_basic_properties_t> amqpProperties() const {
            return nullptr; // [todo]
        }
    };
}