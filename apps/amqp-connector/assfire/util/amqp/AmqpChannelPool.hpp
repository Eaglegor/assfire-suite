#pragma once

#include <amqp.h>
#include "AmqpChannel.hpp"

namespace assfire::util {
    class AmqpChannelPool {
    public:
        AmqpChannel recreateChannel(const AmqpChannel &channel) {
            return AmqpChannel(connection, -1);
        }

        AmqpChannel takeChannel() {
            return AmqpChannel(connection, -1);
        }

        void replaceConnection(amqp_connection_state_t new_connection){
            this->connection = new_connection;
            // [todo] Recreate channels
        }

    private:
        amqp_connection_state_t connection = nullptr;
    };
}