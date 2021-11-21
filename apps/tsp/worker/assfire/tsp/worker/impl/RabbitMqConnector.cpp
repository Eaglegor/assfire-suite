#include "RabbitMqConnector.hpp"
#include <spdlog/spdlog.h>
#include <crossguid/guid.hpp>

namespace assfire::tsp {
    RabbitMqConnector::RabbitMqConnector(const std::string &name) : name(name) {
    }

    namespace {
        void processAmqpError(const amqp_rpc_reply_t &reply, const char *message, bool throw_error = true) {
            switch (reply.reply_type) {
                case AMQP_RESPONSE_NONE: {
                    SPDLOG_ERROR("{}: {}", message, "Missing AMQP reply type");
                    break;
                }

                case AMQP_RESPONSE_LIBRARY_EXCEPTION: {
                    SPDLOG_ERROR("{}: {}", message, amqp_error_string2(reply.library_error));
                    break;
                }

                case AMQP_RESPONSE_SERVER_EXCEPTION: {
                    switch (reply.reply.id) {
                        case AMQP_CONNECTION_CLOSE_METHOD: {
                            amqp_connection_close_t *m =
                                    (amqp_connection_close_t *) reply.reply.decoded;
                            SPDLOG_ERROR("{}: error {} ({})", message, m->reply_code, (const char *) m->reply_text.bytes);
                            break;
                        }
                        case AMQP_CHANNEL_CLOSE_METHOD: {
                            amqp_channel_close_t *m = (amqp_channel_close_t *) reply.reply.decoded;
                            SPDLOG_ERROR("{}: error {} ({})", message, m->reply_code, (const char *) m->reply_text.bytes);
                            break;
                        }
                        default:
                            amqp_channel_close_t *m = (amqp_channel_close_t *) reply.reply.decoded;
                            SPDLOG_ERROR("{}: unknown AMQP server error", message);
                    }
                    break;
                }
            }
            if (throw_error) throw std::runtime_error(message);
        }
    }

    void RabbitMqConnector::connect(const std::string &host, int port, const std::string &login, const std::string &password) {
        SPDLOG_INFO("Connecting RabbitMQ connector {} to {}:{}", name, host, port);

        SPDLOG_INFO("Creating RabbitMQ connection ({})", name);
        connection = amqp_new_connection();
        try {
            socket = amqp_tcp_socket_new(connection);
            if (!socket) {
                SPDLOG_ERROR("Failed to create RabbitMQ socket ({})\", name");
                throw std::runtime_error("Failed to create RabbitMQ socket");
            }

            SPDLOG_INFO("Connecting to RabbitMQ at {}:{} ({})", host, port, name);
            int status = amqp_socket_open(socket, host.c_str(), port);
            if (status != AMQP_STATUS_OK) {
                SPDLOG_ERROR("Failed to connect to RabbitMQ: {} ({})", amqp_error_string2(status), name);
                throw std::runtime_error("Failed to connect to RabbitMQ");
            }

            SPDLOG_INFO("Logging in to RabbitMQ as {} ({})", login.c_str(), name);
            amqp_rpc_reply_t reply = amqp_login(connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, login.c_str(), password.c_str());

            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                processAmqpError(reply, "Failed to log in to RabbitMQ");
            }
        } catch (const std::exception &e) {
            SPDLOG_ERROR("Failed to initialize RabbitMQ transport for ({}). Cleaning up connections...", name);
            amqp_connection_close(connection, AMQP_REPLY_SUCCESS);
            amqp_destroy_connection(connection);
            SPDLOG_ERROR("RabbitMQ cleanup finished");
            throw e;
        }

        SPDLOG_INFO("RabbitMQ connector {} successfully connected to {}:{}", name, host, port);
    }

    void RabbitMqConnector::listen(const std::string &routing_key, const std::string &exchange_name, int channel_id, const MessageCallback &message_callback, bool auto_delete) {
        Listener listener = listen(routing_key, exchange_name, channel_id, auto_delete);
        while (!is_interrupted) {
            listener.next(message_callback);
        }
    }

    void RabbitMqConnector::interrupt() {
        is_interrupted = true;
    }

    RabbitMqConnector::~RabbitMqConnector() {
        SPDLOG_INFO("Destroying RabbitMQ connector {}", name);

        SPDLOG_INFO("Closing RabbitMQ connection...");
        amqp_rpc_reply_t_ reply = amqp_connection_close(connection, AMQP_REPLY_SUCCESS);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to close RabbitMQ connection", false);
        }

        SPDLOG_INFO("Destroying RabbitMQ connection...");
        int status = amqp_destroy_connection(connection);
        if (status != AMQP_STATUS_OK) {
            processAmqpError(reply, "Failed to destroy RabbitMQ connection", false);
        }

        SPDLOG_INFO("RabbitMQ connector {} destroyed", name);
    }

    RabbitMqConnector::Publisher RabbitMqConnector::publish(const std::string &routing_key, const std::string &exchange_name, const std::string &exchange_type, int channel_id, bool auto_delete) {
        return Publisher(name, connection, routing_key, exchange_name, exchange_type, channel_id, auto_delete);
    }

    RabbitMqConnector::Listener RabbitMqConnector::listen(const std::string &routing_key, const std::string &exchange_name, int channel_id, bool auto_delete) {
        SPDLOG_INFO("Starting consuming RabbitMQ messages for queue {} (channel {})", routing_key, channel_id);
        return RabbitMqConnector::Listener(name, connection, routing_key, exchange_name, channel_id, auto_delete);
    }

    void RabbitMqConnector::declareExchange(const std::string &exchange_name, const std::string &exchange_type, int channel_id, bool auto_delete) {
        SPDLOG_INFO("Opening RabbitMQ channel {} for {}", channel_id, name);
        amqp_channel_open(connection, channel_id);
        amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to open RabbitMQ channel");
        }

        SPDLOG_INFO("Declaring RabbitMQ exchange {} ({}) (channel {})", exchange_name, exchange_type, channel_id);
        amqp_exchange_declare(
                connection,
                channel_id,
                amqp_cstring_bytes(exchange_name.c_str()),
                amqp_cstring_bytes(exchange_type.c_str()),
                0,
                0,
                auto_delete ? 1 : 0,
                0,
                amqp_empty_table
                );

        reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to declare RabbitMQ exchange");
        }

        SPDLOG_INFO("Closing RabbitMQ channel {}...", channel_id);
        reply = amqp_channel_close(connection, channel_id, AMQP_REPLY_SUCCESS);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to close RabbitMQ channel", false);
        }
    }

    RabbitMqConnector::Publisher::Publisher(const std::string &name, const amqp_connection_state_t &connection, const std::string &routing_key, const std::string &exchange_name,
                                            const std::string &exchange_type, int channel_id, bool auto_delete) :
            state(std::make_shared<PublisherState>(name,
                                          connection,
                                          routing_key,
                                          exchange_name,
                                          exchange_type,
                                          channel_id,
                                          auto_delete)) {
    }

    void RabbitMqConnector::Publisher::publish(void *bytes, int len) {
        amqp_bytes_t message_bytes;
        message_bytes.len = len;
        message_bytes.bytes = bytes;
        SPDLOG_DEBUG("Sending {} bytes from {} to RabbitMQ queue {} on channel {}", len, state->name, state->routing_key, state->channel_id);
        auto status = amqp_basic_publish(state->connection, state->channel_id, amqp_cstring_bytes(state->exchange_name.c_str()),
                                         amqp_cstring_bytes(state->routing_key.c_str()), 0, 0,
                                         nullptr, message_bytes);
        if (status != AMQP_STATUS_OK) {
            SPDLOG_ERROR("Error while publishing RabbitMQ message: {}", amqp_error_string2(status));
        }
        amqp_rpc_reply_t reply = amqp_get_rpc_reply(state->connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to send message to RabbitMQ");
        }
    }

    RabbitMqConnector::State::State(const std::string &name, const amqp_connection_state_t &connection, const std::string &routing_key, const std::string &exchange_name, int channel_id,
                                    bool auto_delete)
            : name(name), connection(connection), routing_key(routing_key), exchange_name(exchange_name), channel_id(channel_id) {
        SPDLOG_INFO("Opening RabbitMQ channel {} for {}", channel_id, name);
        amqp_channel_open(connection, channel_id);
        amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to open RabbitMQ channel");
        }
    }

    RabbitMqConnector::State::~State() {
        SPDLOG_INFO("Closing RabbitMQ channel {}...", channel_id);
        amqp_rpc_reply_t reply = amqp_channel_close(connection, channel_id, AMQP_REPLY_SUCCESS);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to close RabbitMQ channel", false);
        }
    }

    RabbitMqConnector::Listener::Listener(const std::string &name, const amqp_connection_state_t &connection, const std::string &queue_name, const std::string &exchange_name,
                                          int channel_id, bool auto_delete) :
            state(std::make_shared<ListenerState>(name, connection, queue_name, exchange_name, channel_id, xg::newGuid().str(), auto_delete)) {
    }

    RabbitMqConnector::Listener::~Listener() {

    }

    void RabbitMqConnector::Listener::next(const std::function<void(const amqp_envelope_t &)> &message_callback) {
        amqp_envelope_t envelope;
        amqp_maybe_release_buffers(state->connection);

        SPDLOG_DEBUG("Waiting for next message from {}...", state->routing_key);
        amqp_rpc_reply_t reply = amqp_consume_message(state->connection, &envelope, NULL, 0);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "There was an error while retrieving message from RabbitMQ queue");
        }
        SPDLOG_DEBUG("Received {} bytes from queue {}", envelope.message.body.len, state->routing_key);

        SPDLOG_DEBUG("Passing message from {} to processing callback...", state->routing_key);
        message_callback(envelope);

        amqp_basic_ack(state->connection, state->channel_id, envelope.delivery_tag, false);

        amqp_destroy_envelope(&envelope);
    }

    RabbitMqConnector::ListenerState::ListenerState(const std::string &name, const amqp_connection_state_t &connection, const std::string &queue_name, const std::string &exchange_name, int channel_id,
                                                    const std::string &consumer_tag,
                                                    bool auto_delete)
            : State(name, connection, queue_name, exchange_name, channel_id, auto_delete),
              consumer_tag(consumer_tag) {

        SPDLOG_INFO("Declaring RabbitMQ queue {} (channel {})", queue_name.empty() ? "<auto>" : queue_name, channel_id);
        amqp_queue_declare(
                connection,
                channel_id,
                amqp_cstring_bytes(queue_name.c_str()),
                0,
                0,
                0,
                auto_delete ? 1 : 0,
                amqp_empty_table);
        amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to declare RabbitMQ queue");
        }

        SPDLOG_INFO("Binding RabbitMQ queue {} to exchange {} (channel {})", queue_name.empty() ? "<auto>" : queue_name, exchange_name, channel_id);
        amqp_queue_bind(connection, channel_id, amqp_cstring_bytes(queue_name.c_str()), amqp_cstring_bytes(exchange_name.c_str()),
                        amqp_cstring_bytes(queue_name.c_str()), amqp_empty_table);
        reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to bind RabbitMQ queue");
        }

        SPDLOG_INFO("Subscribing for RabbitMQ queue {} (channel {})", queue_name.empty() ? "<auto>" : queue_name, channel_id);
        amqp_basic_consume(connection,
                           channel_id,
                           amqp_cstring_bytes(queue_name.c_str()),
                           amqp_cstring_bytes(consumer_tag.c_str()),
                           0,
                           0,
                           0,
                           amqp_empty_table);
        reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to subscribe to RabbitMQ queue");
        }
    }

    RabbitMqConnector::ListenerState::~ListenerState() {
        SPDLOG_INFO("Unsubscribing from RabbitMQ queue {} (channel {})", routing_key.empty() ? "<auto>" : routing_key, channel_id);
        amqp_basic_cancel(connection, channel_id, amqp_cstring_bytes(consumer_tag.c_str()));
        amqp_rpc_reply_t_ reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to unsubscribe from RabbitMQ queue");
        }
    }

    RabbitMqConnector::PublisherState::PublisherState(const std::string &name, amqp_connection_state_t const &connection, const std::string &queue_name, const std::string &exchange_name,
                                                      const std::string &exchange_type, int channel_id, bool auto_delete) :
            State(name, connection, queue_name, exchange_name, channel_id, auto_delete){
        SPDLOG_INFO("Declaring RabbitMQ exchange {} ({}) (channel {})", exchange_name, exchange_type, channel_id);
        amqp_exchange_declare(
                connection,
                channel_id,
                amqp_cstring_bytes(exchange_name.c_str()),
                amqp_cstring_bytes(exchange_type.c_str()),
                0,
                0,
                auto_delete ? 1 : 0,
                0,
                amqp_empty_table
        );
        amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            processAmqpError(reply, "Failed to declare RabbitMQ exchange");
        }
    }

    RabbitMqConnector::PublisherState::~PublisherState() {

    }
}