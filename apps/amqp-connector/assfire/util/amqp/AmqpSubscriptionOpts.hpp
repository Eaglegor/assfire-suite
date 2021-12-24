#pragma once

#include <amqp.h>

namespace assfire::util
{
    struct AmqpSubscriptionOpts
    {
        std::string queue_name;
        bool no_local;
        bool no_ack;
        bool exclusive;

        amqp_bytes_t bytesQueueName() const {
            return amqp_cstring_bytes(queue_name.c_str());
        }

        int intNoLocal() const {
            return no_local ? 1 : 0;
        }

        int inNoAck() const {
            return no_ack ? 1 : 0;
        }

        int inExclusive() const {
            return exclusive ? 1 : 0;
        }

        amqp_table_t amqpArgs() const {
            return amqp_empty_table;
        }
    };
}