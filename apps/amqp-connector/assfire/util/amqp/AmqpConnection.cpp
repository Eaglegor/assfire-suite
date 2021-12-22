#include "AmqpConnection.hpp"

#include <stdexcept>
#include <chrono>

using namespace std::literals::chrono_literals;

namespace assfire::util {

    static const int RECONNECT_BACKOFF_ATTEMPTS = 5;
    static const auto RECONNECT_BACKOFF_INTERVAL = 5s;

    AmqpConnection::AmqpConnection(const std::string &name, const AmqpConnectionOpts &options)
            : name(name), options(options), state(State::NOT_CONNECTED) {
    }

    AmqpConnection::~AmqpConnection() {
        amqp_destroy_connection(connection);
    }

    void AmqpConnection::connect() {
        std::lock_guard<std::mutex> guard(mtx);
        if (state == State::CONNECTED) return;
        state = State::CONNECTING;

        try {
            connection = amqp_new_connection();
            socket = amqp_tcp_socket_new(connection);
            if (!socket) {
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "failed to create amqp socket"));
            }

            int status = amqp_socket_open(socket, options.host.c_str(), options.port);
            if (status != AMQP_STATUS_OK) {
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "failed to open amqp socket"));
            }

            amqp_rpc_reply_t rpl = amqp_login(connection, "/", 0, 0, 0, AMQP_SASL_METHOD_PLAIN, options.login.c_str(), options.password.c_str());
            if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
                throw amqp_exception(AmqpError::fromReply(rpl));
            }

            state = State::CONNECTED;

            channels.replaceConnection(connection);
            // [todo] recreate resources

            return;
        } catch (const std::exception &e) {
            amqp_destroy_connection(connection);
            state = State::NOT_CONNECTED;
            throw e;
        }
    }

    void AmqpConnection::declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts) {
        AmqpChannel channel = takeChannel();

        try {
            channel.declareExchange(exchange_opts);
        } catch (const amqp_exception &e) {
            recoverChannel(e.getError(), channel).declareExchange(exchange_opts);
        }
        declared_exchanges.emplace(name, exchange_opts);

    }

    std::string AmqpConnection::declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts) {
        AmqpChannel channel = takeChannel();

        std::string actual_name;
        try {
            actual_name = channel.declareQueue(queue_opts);
        }
        catch (const amqp_exception &e) {
            actual_name = recoverChannel(e.getError(), channel).declareQueue(queue_opts);
        }

        declared_queues.emplace(name, queue_opts);
        if (actual_name != name) {
            queue_name_mappings.emplace(name, actual_name);
        }

        return actual_name;
    }

    void AmqpConnection::bindQueue(const AmqpQueueBinding &queue_binding) {
        AmqpChannel channel = takeChannel();

        AmqpQueueBinding mapped_binding(queue_binding);
        if (queue_name_mappings.contains(queue_binding.queue_name)) {
            mapped_binding.queue_name = queue_name_mappings.at(queue_binding.queue_name);
        }

        try {
            channel.bindQueue(mapped_binding);
        }
        catch (const amqp_exception &e) {
            recoverChannel(e.getError(), channel).bindQueue(mapped_binding);
        }
        bound_queues.emplace_back(queue_binding);
    }

    AmqpChannel AmqpConnection::takeChannel() {
        return channels.takeChannel();
    }

    AmqpChannel AmqpConnection::recoverChannel(const AmqpError &error, const AmqpChannel &channel) {
        switch (error.type) {
            case AmqpErrorType::CONNECTION_CLOSED:
                for (int i = 0; i < RECONNECT_BACKOFF_ATTEMPTS; ++i) {
                    try {
                        connect();
                        return channels.takeChannel();
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
                throw amqp_exception(AmqpError(AmqpErrorType::UNKNOWN, "Failed to recover AMQP channel after error"));
        }
    }
}