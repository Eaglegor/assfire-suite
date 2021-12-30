#include "AmqpConnection.hpp"

#include <stdexcept>
#include <chrono>
#include <utility>
#include <thread>

using namespace std::literals::chrono_literals;

namespace assfire::util
{

    static const int RECONNECT_BACKOFF_ATTEMPTS = 20;
    static const auto RECONNECT_BACKOFF_INTERVAL = 5s;

    AmqpConnection::AmqpConnection(const std::string &name, const AmqpConnectionOpts &options)
            : name(name), options(options), state(State::NOT_CONNECTED), channels(name) {
    }

    AmqpConnection::~AmqpConnection() {
        amqp_destroy_connection(connection);
    }

    amqp_connection_state_t AmqpConnection::createConnection() const {
        amqp_connection_state_t new_connection = nullptr;
        try {
            new_connection = amqp_new_connection();
            if (!new_connection) {
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "failed to create amqp connection"));
            }
            amqp_socket_t *new_socket = amqp_tcp_socket_new(new_connection);
            if (!new_socket) {
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "failed to create amqp socket"));
            }

            int status = amqp_socket_open(new_socket, options.host.c_str(), options.port);
            if (status != AMQP_STATUS_OK) {
                SPDLOG_ERROR("Error while trying to open AMQP socket: {}", amqp_error_string2(status));
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "failed to open amqp socket"));
            }

            amqp_rpc_reply_t rpl = amqp_login(new_connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, options.login.c_str(), options.password.c_str());
            if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
                AmqpError error = AmqpError::fromReply(rpl);
                SPDLOG_ERROR("Error while trying to login to AMQP broker: {}", error.message);
                throw amqp_exception(error);
            }

            return new_connection;
        } catch (const amqp_exception &e) {
            if (new_connection) {
                amqp_destroy_connection(new_connection);
            }
            throw e;
        }
    }

    void AmqpConnection::declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts) {
        executeWithAutoReconnect([&](const auto &ch) {
            ch.declareExchange(exchange_opts);
        });

        declared_exchanges[name] = exchange_opts;
    }

    std::string AmqpConnection::declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts) {
        std::string actual_name;

        executeWithAutoReconnect([&](const auto &ch) {
            actual_name = ch.declareQueue(queue_opts);
        });

        SPDLOG_DEBUG("Declared queue: {}->{}", name, actual_name);

        declared_queues[name] = queue_opts;
        if (actual_name != name) {
            queue_name_mappings[name] = actual_name;
        }

        return name;
    }

    void AmqpConnection::releaseQueue(const std::string &name) {
        const auto &opts = declared_queues.at(name);
        if (!opts.auto_delete) {
            executeWithAutoReconnect([&](const auto &ch) {
                ch.deleteQueue(name, true, false);
            });
        }
        queue_name_mappings.erase(name);
        declared_queues.erase(name);
    }

    void AmqpConnection::bindQueue(const AmqpQueueBinding &queue_binding) {
        AmqpQueueBinding mapped_binding(queue_binding);
        mapped_binding.queue_name = resolveQueueName(queue_binding.queue_name);

        executeWithAutoReconnect([&](const auto &ch) {
            ch.bindQueue(mapped_binding);
        });

        bound_queues.emplace_back(queue_binding);
    }

    void AmqpConnection::publish(const std::string &bytes, const AmqpEnvelopeOpts &options) {
        executeWithAutoReconnect([&](const auto &ch) {
            ch.publish(bytes, options);
        });
    }

    std::string AmqpConnection::subscribe(const AmqpSubscriptionOpts &options) {
        auto channel = takeChannel();

        AmqpSubscriptionOpts mapped_options(options);
        mapped_options.queue_name = resolveQueueName(options.queue_name);

        std::string consumer_id;
        executeWithAutoReconnect(channel, [&](const auto &ch) {
            consumer_id = ch.subscribe(mapped_options);
        });

        subscriptions[consumer_id] = std::make_pair(options, channel);
        return consumer_id;
    }

    std::string AmqpConnection::subscribe(const std::string &consumer_id, const AmqpSubscriptionOpts &options) {
        auto channel = takeChannel();

        AmqpSubscriptionOpts mapped_options(options);
        mapped_options.queue_name = resolveQueueName(options.queue_name);

        executeWithAutoReconnect(channel, [&](const auto &ch) {
            ch.subscribe(consumer_id, mapped_options);
        });

        subscriptions[consumer_id] = std::make_pair(options, channel);
        return consumer_id;
    }

    void AmqpConnection::consumeMessage(const std::string &consumer_id, AmqpChannel::MessageCallback callback) {
        const auto &subscription = subscriptions.at(consumer_id);

        executeWithAutoReconnect(subscription.second, [&](const auto &ch) {
            ch.consumeMessage(callback);
        });
    }

    void AmqpConnection::consumeMessage(const std::string &consumer_id, AmqpChannel::MessageCallback callback, std::chrono::milliseconds timeout) {
        const auto &subscription = subscriptions.at(consumer_id);

        executeWithAutoReconnect(subscription.second, [&](const auto &ch) {
            ch.consumeMessage(callback, timeout);
        });
    }

    void AmqpConnection::unsubscribe(const std::string &consumer_id) {
        const auto &subscription = subscriptions.at(consumer_id);

        executeWithAutoReconnect(subscription.second, [&](const auto &ch) {
            ch.unsubscribe(consumer_id);
        });

        recycleChannel(subscription.second);
        subscriptions.erase(consumer_id);
    }

    void AmqpConnection::executeWithAutoReconnect(const AmqpChannel &channel, std::function<void(const AmqpChannel &)> action) {
        AmqpChannel ch = channel;
        for (int i = 0; i < RECONNECT_BACKOFF_ATTEMPTS; ++i) {
            try {
                action(ch);
                return;
            } catch (const amqp_exception &e) {
                if (state == State::RECONNECTING || i == RECONNECT_BACKOFF_ATTEMPTS - 1) {
                    throw e;
                }
                ch = recoverChannel(e.getError(), ch);
            }
        }
    }

    void AmqpConnection::executeWithAutoReconnect(std::function<void(const AmqpChannel &)> action) {
        auto channel = takeChannel();
        executeWithAutoReconnect(channel, std::move(action));
        recycleChannel(channel);
    }

    AmqpChannel AmqpConnection::takeChannel() {
        if (state == State::NOT_CONNECTED) connect();
        return channels.takeChannel();
    }

    void AmqpConnection::recycleChannel(AmqpChannel channel) {
        channels.recycleChannel(channel);
    }

    AmqpChannel AmqpConnection::recoverChannel(const AmqpError &error, const AmqpChannel &channel) {
        SPDLOG_INFO("Trying to recover channel {}/{} after error: {}", name, channel.getId(), error.message);
        switch (error.type) {
            case AmqpErrorType::CONNECTION_CLOSED:
                for (int i = 0; i < RECONNECT_BACKOFF_ATTEMPTS; ++i) {
                    try {
                        state = State::RECONNECTING;
                        SPDLOG_INFO("Trying to connect to AMQP broker at {}:{}, ({})", options.host, options.port, name);
                        replaceConnection(createConnection());
                        SPDLOG_INFO("Recreating topology for ({})", name);
                        recreateTopology();
                        state = State::CONNECTED;
                        SPDLOG_INFO("Successfully connected to AMQP broker ({})", name);
                        return channels.recreateChannel(channel);
                    } catch (const amqp_exception &e) {
                        state = State::NOT_CONNECTED;
                        std::this_thread::sleep_for(RECONNECT_BACKOFF_INTERVAL);
                        if (i == RECONNECT_BACKOFF_ATTEMPTS - 1) throw e;
                    } catch (const std::exception &e) {
                        SPDLOG_ERROR("Unexpected exception in reconnection loop: {}", e.what());
                        throw e;
                    }
                }
            case AmqpErrorType::CHANNEL_CLOSED:
                for (int i = 0; i < RECONNECT_BACKOFF_ATTEMPTS; ++i) {
                    try {
                        return channels.recreateChannel(channel);
                    } catch (const amqp_exception &e) {
                        std::this_thread::sleep_for(RECONNECT_BACKOFF_INTERVAL);
                        if (i == RECONNECT_BACKOFF_ATTEMPTS - 1) throw e;
                    }
                }
            default:
                // Couldn't recover channel
                return channel;
        }
    }

    void AmqpConnection::connect() {
        SPDLOG_INFO("Trying to establish connection to AMQP broker at {}:{}, ({})", options.host, options.port, name);
        try {
            replaceConnection(createConnection());
        } catch (const amqp_exception &e) {
            SPDLOG_ERROR("Failed to establish connection to AMQP broker ({}): {}", name, e.what());
            throw e;
        }
        SPDLOG_INFO("Successfully connected to AMQP broker ({})", name);
        state = State::CONNECTED;
    }

    void AmqpConnection::recreateTopology() {
        // Recreate exchanges
        auto exchanges_to_restore = declared_exchanges;
        declared_exchanges.clear();
        for (const auto &exchange: exchanges_to_restore) {
            declareExchange(exchange.first, exchange.second);
        }

        // Recreate queues
        queue_name_mappings.clear();
        auto queues_to_restore = declared_queues;
        declared_queues.clear();
        for (const auto &queue: declared_queues) {
            declareQueue(queue.first, queue.second);
        }

        // Recreate bindings
        auto bindings_to_restore = bound_queues;
        auto iter = std::remove_if(bindings_to_restore.begin(), bindings_to_restore.end(), [&](const auto &bnd) {
            return !declared_queues.contains(bnd.queue_name);
        });
        bindings_to_restore.erase(iter, bindings_to_restore.end());
        bound_queues.clear();
        for (const auto &binding: bindings_to_restore) {
            bindQueue(binding);
        }

        // Recreate subscriptions
        auto subscriptions_to_restore = subscriptions;
        subscriptions.clear();
        for (auto &sub: subscriptions_to_restore) {
            subscribe(sub.first, sub.second.first);
        }

    }

    void AmqpConnection::replaceConnection(amqp_connection_state_t new_connection) {
        connection = new_connection;
        channels.replaceConnection(connection);
    }

    std::string AmqpConnection::resolveQueueName(const std::string &name) const {
        auto iter = queue_name_mappings.find(name);
        if (iter == queue_name_mappings.end()) return name;
        return iter->second;
    }
}