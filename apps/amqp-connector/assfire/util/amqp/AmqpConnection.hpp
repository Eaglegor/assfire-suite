#pragma once

#include <string>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <optional>
#include "AmqpChannelPool.hpp"
#include "AmqpConnectionOpts.hpp"
#include "AmqpExchangeOpts.hpp"
#include "AmqpQueueOpts.hpp"
#include "AmqpChannel.hpp"
#include "AmqpQueueBinding.hpp"

namespace assfire::util {
    class AmqpConnection {
    public:
        AmqpConnection(const std::string &name, const AmqpConnectionOpts &options);

        ~AmqpConnection();

        bool connect();

        bool declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts);

        bool declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts);

        bool bindQueue(const AmqpQueueBinding &queue_binding);

    private:
        enum class State {
            NOT_CONNECTED,
            CONNECTING,
            CONNECTED
        };

        bool ensureConnected();
        ChannelRef takeChannel();

        std::string name;
        AmqpConnectionOpts options;

        std::mutex mtx;
        std::atomic<State> state;
        amqp_socket_t *socket = nullptr;
        amqp_connection_state_t connection;

        std::unordered_map<std::string, AmqpExchangeOpts> declared_exchanges;
        std::unordered_map<std::string, AmqpQueueOpts> declared_queues;
        std::unordered_map<std::string, std::string> queue_name_mappings;
        std::vector<AmqpQueueBinding> bound_queues;
        AmqpChannelPool channels;
    };
}