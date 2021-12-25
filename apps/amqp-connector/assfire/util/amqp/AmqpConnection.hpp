#pragma once

#include <string>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <optional>
#include <vector>
#include <functional>
#include <utility>
#include "AmqpChannelPool.hpp"
#include "AmqpConnectionOpts.hpp"
#include "AmqpExchangeOpts.hpp"
#include "AmqpQueueOpts.hpp"
#include "AmqpChannel.hpp"
#include "AmqpQueueBinding.hpp"
#include "AmqpEnvelopeOpts.hpp"

namespace assfire::util
{
    class AmqpConnection
    {
    public:
        AmqpConnection(const std::string &name, const AmqpConnectionOpts &options);

        ~AmqpConnection();

        void declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts);

        std::string declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts);

        void bindQueue(const AmqpQueueBinding &queue_binding);

        void publish(const std::string &bytes, const AmqpEnvelopeOpts &options);

        std::string subscribe(const AmqpSubscriptionOpts &options);

        void consumeMessage(const std::string &consumer_id, AmqpChannel::MessageCallback callback);

        void consumeMessage(const std::string &consumer_id, AmqpChannel::MessageCallback callback, std::chrono::milliseconds timeout);

        void unsubscribe(const std::string &consumer_id);

    private:
        void connect();

        amqp_connection_state_t createConnection() const;

        void replaceConnection(amqp_connection_state_t new_connection);

        std::string subscribe(const std::string &consumer_id, const AmqpSubscriptionOpts &options);

        void recreateTopology();

        void executeWithAutoReconnect(const AmqpChannel &channel, std::function<void(const AmqpChannel &)>);

        void executeWithAutoReconnect(std::function<void(const AmqpChannel &)>);

        AmqpChannel takeChannel();

        void recycleChannel(AmqpChannel channel);

        AmqpChannel recoverChannel(const AmqpError &error, const AmqpChannel &channel);

        enum class State {
            NOT_CONNECTED,
            CONNECTED,
            RECONNECTING
        };

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
        std::unordered_map<std::string, std::pair<AmqpSubscriptionOpts, AmqpChannel>> subscriptions;
    };
}