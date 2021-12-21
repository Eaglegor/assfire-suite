#pragma once

#include <string>
#include <amqp.h>

namespace assfire::util {
    struct AmqpQueueOpts {
        std::string name;
        bool passive;
        bool durable;
        bool exclusive;
        bool auto_delete;
        // [todo] additional arguments

        amqp_bytes_t bytesName() const {
            return amqp_cstring_bytes(name.c_str());
        }

        int intPassive() const {
            return passive ? 1 : 0;
        }

        int intDurable() const {
            return durable ? 1 : 0;
        }

        int intAutoDelete() const {
            return auto_delete ? 1 : 0;
        }

        int intExclusive() const {
            return exclusive ? 1 : 0;
        }

        amqp_table_t amqpArgs() const {
            return amqp_empty_table;
        }
    };
}