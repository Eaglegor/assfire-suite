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
            State(const std::string &name, const amqp_connection_state_t &connection, const std::string &queue_name, const std::string &exchange_name, int channel_id);
            ~State();

            std::string name;
            amqp_connection_state_t connection;
            std::string queue_name;
            std::string exchange_name;
            int channel_id;
        };

    public:
        class Publisher {
        public:
            Publisher(const std::string& name, const amqp_connection_state_t &connection, const std::string &queue_name, const std::string &exchange_name, int channel_id);

            void publish(void* bytes, int len);

        private:
            std::shared_ptr<State> state;
        };

        class Listener {
        public:
            Listener(const std::string& name, const amqp_connection_state_t &connection, const std::string &queue_name, const std::string &exchange_name, int channel_id);
            void next(const MessageCallback& message_callback);

        private:
            std::shared_ptr<State> state;
        };

        explicit RabbitMqConnector(const std::string &name);

        virtual ~RabbitMqConnector();

        void connect(const std::string &host, int port, const std::string &login, const std::string &password);
        void listen(const std::string &queue_name, const std::string &exchange_name, int channel_id, const MessageCallback& message_callback);
        Listener listen(const std::string &queue_name, const std::string &exchange_name, int channel_id);

        Publisher publish(const std::string &queue_name, const std::string &exchange_name, int channel_id);

        void interrupt();

    private:
        std::string name;
        amqp_socket_t *socket = nullptr;
        amqp_connection_state_t connection;
        std::atomic_bool is_interrupted = false;
    };
}