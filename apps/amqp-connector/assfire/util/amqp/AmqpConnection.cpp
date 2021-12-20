#include "AmqpConnection.hpp"

#include <stdexcept>

namespace assfire::util {

    AmqpConnection::AmqpConnection(const std::string &name, const AmqpConnectionOpts &options)
            : name(name), options(options), state(State::NOT_CONNECTED) {
    }

    AmqpConnection::~AmqpConnection() {
        amqp_destroy_connection(connection);
    }

    bool AmqpConnection::connect() {
        std::lock_guard<std::mutex> guard(mtx);
        if (state == State::CONNECTED) return true;
        state = State::CONNECTING;

        try {
            connection = amqp_new_connection();
            socket = amqp_tcp_socket_new(connection);
            if (!socket) {
                throw std::runtime_error("failed to create amqp socket");
            }

            int status = amqp_socket_open(socket, options.host.c_str(), options.port);
            if (status != AMQP_STATUS_OK) {
                throw std::runtime_error("failed to open amqp socket");
            }

            amqp_rpc_reply_t rpl = amqp_login(connection, "/", 0, 0, 0, AMQP_SASL_METHOD_PLAIN, options.login.c_str(), options.password.c_str());

            if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
                throw std::runtime_error("failed to login to amqp server");
            }

            state = State::CONNECTED;
            return true;
        } catch (const std::exception &e) {
            amqp_destroy_connection(connection);
            state = State::NOT_CONNECTED;
            return false;
        }
    }

    bool AmqpConnection::declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts) {
        if (!ensureConnected()) {
            return false;
        }
        ChannelRef channel = takeChannel();

        amqp_exchange_declare(
                connection,
                channel.getId(),
                exchange_opts.bytesName(),
                exchange_opts.bytesType(),
                exchange_opts.intPassive(),
                exchange_opts.intDurable(),
                exchange_opts.intAutoDelete(),
                exchange_opts.intInternal(),
                exchange_opts.amqpArgs()
        );

        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            return false;
        }
        declared_exchanges.emplace(name, exchange_opts);
        return true;
    }

    bool AmqpConnection::declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts) {
        if (!ensureConnected()) {
            return false;
        }
        ChannelRef channel = takeChannel();

        amqp_queue_declare(
                connection,
                channel.getId(),
                queue_opts.bytesName(),
                queue_opts.intPassive(),
                queue_opts.intDurable(),
                queue_opts.intExclusive(),
                queue_opts.intAutoDelete(),
                queue_opts.amqpArgs()
        );

        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            return false;
        }
        declared_queues.emplace(name, queue_opts);
        return true;
    }

    bool AmqpConnection::bindQueue(const AmqpQueueBinding &queue_binding) {
        if (!ensureConnected()) {
            return false;
        }
        ChannelRef channel = takeChannel();

        amqp_queue_bind(
                connection,
                channel.getId(),
                queue_binding.bytesQueueName(),
                queue_binding.bytesExchangeName(),
                queue_binding.bytesRoutingKey(),
                queue_binding.amqpArgs()
        );


        auto rpl = amqp_get_rpc_reply(connection);
        if (rpl.reply_type != AMQP_RESPONSE_NORMAL) {
            return false;
        }
        bound_queues.emplace_back(queue_binding);
        return true;
    }

    bool AmqpConnection::ensureConnected() {
        if (state.load() != State::CONNECTED) {
            return connect();
        }
        return true;
    }

    ChannelRef AmqpConnection::takeChannel() {
        return channels.takeChannel();
    }
}