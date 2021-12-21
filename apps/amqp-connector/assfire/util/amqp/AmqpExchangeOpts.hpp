#pragma once

#include <string>
#include <amqp.h>

namespace assfire::util {
    enum class AmqpExchangeType {
        Direct,
        Topic,
        Fanout
    };

    struct AmqpExchangeOpts {
        std::string name;
        AmqpExchangeType type;
        bool passive;
        bool durable;
        bool auto_delete;
        bool internal;
        // [todo] additional arguments

        const char* stringType() const {
            switch (type) {
                case AmqpExchangeType::Direct:
                    return "amq.direct";
                case AmqpExchangeType::Topic:
                    return "amq.topic";
                case AmqpExchangeType::Fanout:
                    return "amq.fanout";
            }
            return "amq.direct"; // [todo] default?
        }

        amqp_bytes_t bytesName() const {
            return amqp_cstring_bytes(name.c_str());
        }

        amqp_bytes_t bytesType() const {
            return amqp_cstring_bytes(stringType());
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

        int intInternal() const {
            return internal ? 1 : 0;
        }

        amqp_table_t amqpArgs() const {
            return amqp_empty_table;
        }
    };
}