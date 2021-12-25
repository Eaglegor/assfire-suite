#include "AmqpConnection.hpp"

#include <stdexcept>
#include <chrono>
#include <utility>

using namespace std::literals::chrono_literals;

namespace assfire::util
{

    static const int RECONNECT_BACKOFF_ATTEMPTS = 5;
    static const auto RECONNECT_BACKOFF_INTERVAL = 5s;

    AmqpConnection::AmqpConnection(const std::string &name, const AmqpConnectionOpts &options)
            : name(name), options(options), state(State::NOT_CONNECTED) {
    }

    AmqpConnection::~AmqpConnection() {
        amqp_destroy_connection(connection);
    }

    amqp_connection_state_t AmqpConnection::createConnection() const {
        amqp_connection_state_t new_connection = nullptr;
        try {
            new_connection = amqp_new_connection();
            amqp_socket_t *new_socket = amqp_tcp_socket_new(new_connection);
            if (!new_socket) {
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "failed to create amqp socket"));
            }

            int status = amqp_socket_open(new_socket, options.host.c_str(), options.port);
            if (status != AMQP_STATUS_OK) {
                SPDLOG_ERROR("Error while trying to open AMQP socket: {}", amqp_error_string2(status));
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "failed to open amqp socket"));
            }

            amqp_rpc_reply_t rpl = amqp_login(new_connection, "/", 0, 0, 0, AMQP_SASL_METHOD_PLAIN, options.login.c_str(), options.password.c_str());
            if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
                throw amqp_exception(AmqpError::fromReply(rpl));
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

        declared_exchanges.emplace(name, exchange_opts);
    }

    std::string AmqpConnection::declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts) {
        std::string actual_name;

        executeWithAutoReconnect([&](const auto &ch) {
            actual_name = ch.declareQueue(queue_opts);
        });

        declared_queues.emplace(name, queue_opts);
        if (actual_name != name) {
            queue_name_mappings.emplace(name, actual_name);
        }

        return actual_name;
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
        if (queue_name_mappings.contains(queue_binding.queue_name)) {
            mapped_binding.queue_name = queue_name_mappings.at(queue_binding.queue_name);
        }

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

        std::string consumer_id;
        executeWithAutoReconnect(channel, [&](const auto &ch) {
            consumer_id = ch.subscribe(options);
        });

        subscriptions.emplace(consumer_id, std::make_pair(options, channel));
        return consumer_id;
    }

    std::string AmqpConnection::subscribe(const std::string &consumer_id, const AmqpSubscriptionOpts &options) {
        auto channel = takeChannel();

        executeWithAutoReconnect(channel, [&](const auto &ch) {
            ch.subscribe(consumer_id, options);
        });

        subscriptions.emplace(consumer_id, std::make_pair(options, channel));
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
        try {
            action(channel);
        }
        catch (const amqp_exception &e) {
            if (state != State::RECONNECTING) {
                action(recoverChannel(e.getError(), channel));
            } else {
                throw e;
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
                state = State::RECONNECTING;
                for (int i = 0; i < RECONNECT_BACKOFF_ATTEMPTS; ++i) {
                    try {
                        replaceConnection(createConnection());
                        recreateTopology();
                        state = State::CONNECTED;
                        return channels.recreateChannel(channel);
                    } catch (const amqp_exception &e) {
                        std::this_thread::sleep_for(RECONNECT_BACKOFF_INTERVAL);
                        if (i == RECONNECT_BACKOFF_ATTEMPTS - 1) throw e;
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
            case AmqpErrorType::UNKNOWN:
            default:
                SPDLOG_ERROR("Failed to recover AMQP channel {}/{} after error: {}", name, channel.getId());
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "Failed to recover AMQP channel after error"));
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
        for (const auto &exchange: declared_exchanges) {
            declareExchange(exchange.first, exchange.second);
        }

        // Recreate queues
        queue_name_mappings.clear();
        for (const auto &queue: declared_queues) {
            declareQueue(queue.first, queue.second);
        }

        // Recreate bindings
        std::remove_if(bound_queues.begin(), bound_queues.end(), [&](const auto &bnd) {
            return !declared_queues.contains(bnd.queue_name);
        });
        for (const auto &binding: bound_queues) {
            bindQueue(binding);
        }

        // Recreate subscriptions
        for (auto &sub: subscriptions) {
            subscribe(sub.first, sub.second.first);
        }

    }

    void AmqpConnection::replaceConnection(amqp_connection_state_t new_connection) {
        connection = new_connection;
        channels.replaceConnection(connection);
    }
}