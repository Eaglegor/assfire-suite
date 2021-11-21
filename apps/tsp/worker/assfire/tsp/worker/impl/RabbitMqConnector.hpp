#pragma once

#include <string>
#include <atomic>
#include <functional>
#include <amqp.h>
#include <memory>
#include <amqp_tcp_socket.h>

namespace assfire::tsp {
    class RabbitMqConnector {
    public:
        using MessageCallback = std::function<void(const amqp_envelope_t &)>;

    private:
        struct State {
            State(const std::string &name, const amqp_connection_state_t &connection, const std::string &routing_key, const std::string &exchange_name, int channel_id, bool auto_delete = false);

            ~State();

            std::string name;
            amqp_connection_state_t connection;
            std::string routing_key;
            std::string exchange_name;
            int channel_id;
        };

        struct ListenerState : public State {
            ListenerState(const std::string &name, const amqp_connection_state_t &connection, const std::string &queue_name, const std::string &exchange_name, int channel_id, const std::string &consumer_tag,
                          bool auto_delete = false);

            ~ListenerState();

            std::string consumer_tag;
        };

        struct PublisherState : public State {
            PublisherState(const std::string &name, const amqp_connection_state_t &connection, const std::string &queue_name, const std::string &exchange_name, const std::string &exchange_type, int channel_id,
                          bool auto_delete = false);

            ~PublisherState();
        };

    public:
        class Publisher {
        public:
            Publisher(const std::string &name, const amqp_connection_state_t &connection, const std::string &routing_key, const std::string &exchange_name, const std::string &exchange_type, int channel_id, bool auto_delete = false);

            void publish(void *bytes, int len);

        private:
            std::shared_ptr<PublisherState> state;
        };

        class Listener {
        public:
            Listener(const std::string &name, const amqp_connection_state_t &connection, const std::string &queue_name, const std::string &exchange_name, int channel_id, bool auto_delete = false);

            virtual ~Listener();

            void next(const MessageCallback &message_callback);

        private:
            std::shared_ptr<ListenerState> state;
        };

        explicit RabbitMqConnector(const std::string &name);

        virtual ~RabbitMqConnector();

        void connect(const std::string &host, int port, const std::string &login, const std::string &password);

        void declareExchange(const std::string &exchange_name, const std::string& exchange_type, int channel_id, bool auto_delete = false);

        void listen(const std::string &routing_key, const std::string &exchange_name, int channel_id, const MessageCallback &message_callback, bool auto_delete = false);

        Listener listen(const std::string &routing_key, const std::string &exchange_name, int channel_id, bool auto_delete = false);

        Publisher publish(const std::string &routing_key, const std::string &exchange_name, const std::string &exchange_type, int channel_id, bool auto_delete = false);

        void interrupt();

    private:
        std::string name;
        amqp_socket_t *socket = nullptr;
        amqp_connection_state_t connection;
        std::atomic_bool is_interrupted = false;
    };
}