#pragma once

#include <memory>
#include <queue>
#include <mutex>
#include "AmqpConnection.hpp"
#include "AmqpPublisher.hpp"
#include "AmqpConsumer.hpp"

namespace assfire::util
{
    class AmqpConnectionPool
    {
    public:
        template<typename T>
        class ConnectionConsumerRef
        {
        public:
            ConnectionConsumerRef()
                    : connection_pool(nullptr) {}

            ConnectionConsumerRef(std::unique_ptr<T> connection_consumer, AmqpConnectionPool &connection_pool, std::shared_ptr<AmqpConnection> connection)
                    : connection_consumer(std::move(connection_consumer)), connection_pool(&connection_pool), connection(std::move(connection)) {}

            ConnectionConsumerRef<T> &operator=(ConnectionConsumerRef &&rhs) {
                connection_consumer = std::move(rhs.connection_consumer);
                connection_pool = rhs.connection_pool;
                rhs.connection_pool = nullptr;
                connection = std::move(rhs.connection);
                return *this;
            };

            T *operator->() {
                return connection_consumer.get();
            }

            operator bool() const {
                return connection_consumer;
            }

            ~ConnectionConsumerRef() {
                if (connection) {
                    connection_pool->returnConnection(std::move(connection));
                }
            }

        private:
            std::unique_ptr<T> connection_consumer;
            AmqpConnectionPool *connection_pool;
            std::shared_ptr<AmqpConnection> connection;
        };

        using PublisherRef = ConnectionConsumerRef<AmqpPublisher>;
        using ConsumerRef = ConnectionConsumerRef<AmqpConsumer>;

        friend class ConnectionConsumerRef<AmqpPublisher>;

        friend class ConnectionConsumerRef<AmqpConsumer>;

        AmqpConnectionPool(AmqpConnectionOpts options);

        void declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts);

        std::string declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts);

        void releaseQueue(const std::string &name);

        void bindQueue(const AmqpQueueBinding &queue_binding);

        PublisherRef createPublisher(const std::string &name, const AmqpPublisherOpts &opts);

        ConsumerRef createConsumer(const std::string &name, const AmqpConsumerOpts &opts);

    private:
        std::shared_ptr<AmqpConnection> takeConnection();

        void returnConnection(std::shared_ptr<AmqpConnection> connection);

        std::mutex mtx;
        AmqpConnectionOpts options;
        std::queue<std::shared_ptr<AmqpConnection>> connections;
    };
}