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
            ConnectionConsumerRef(std::unique_ptr<T> connection_consumer, AmqpConnectionPool &connection_pool, std::shared_ptr<AmqpConnection> connection)
                    : connection_consumer(std::move(connection_consumer)), connection_pool(connection_pool), connection(std::move(connection)) {}

            T *operator->() {
                return connection_consumer.get();
            }

            ~ConnectionConsumerRef() {
                if (connection) {
                    connection_pool.returnConnection(std::move(connection));
                }
            }

        private:
            std::unique_ptr<T> connection_consumer;
            AmqpConnectionPool &connection_pool;
            std::shared_ptr<AmqpConnection> connection;
        };

        using PublisherRef = ConnectionConsumerRef<AmqpPublisher>;
        using ConsumerRef = ConnectionConsumerRef<AmqpConsumer>;

        friend class ConnectionConsumerRef<AmqpPublisher>;

        friend class ConnectionConsumerRef<AmqpConsumer>;


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