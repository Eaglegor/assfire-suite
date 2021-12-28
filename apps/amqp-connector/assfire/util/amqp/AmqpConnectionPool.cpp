#include "AmqpConnectionPool.hpp"
#include <crossguid/guid.hpp>
#include <utility>

namespace assfire::util
{
    AmqpConnectionPool::PublisherRef AmqpConnectionPool::createPublisher(const std::string &name, const AmqpPublisherOpts &opts) {
        auto connection = takeConnection();
        auto &connection_ref = *connection;
        return {std::make_unique<AmqpPublisher>(name, opts, connection_ref), *this, std::move(connection)};
    }

    AmqpConnectionPool::ConsumerRef AmqpConnectionPool::createConsumer(const std::string &name, const AmqpConsumerOpts &opts) {
        auto connection = takeConnection();
        auto &connection_ref = *connection;
        return {std::make_unique<AmqpConsumer>(name, opts, connection_ref), *this, std::move(connection)};
    }

    void AmqpConnectionPool::declareExchange(const std::string &name, const AmqpExchangeOpts &exchange_opts) {
        auto connection = takeConnection();
        try {
            connection->declareExchange(name, exchange_opts);
            returnConnection(connection);
        } catch (const amqp_exception &e) {
            returnConnection(connection);
            throw e;
        }
    }

    std::string AmqpConnectionPool::declareQueue(const std::string &name, const AmqpQueueOpts &queue_opts) {
        auto connection = takeConnection();
        try {
            std::string queue_name = connection->declareQueue(name, queue_opts);
            returnConnection(connection);
            return queue_name;
        } catch (const amqp_exception &e) {
            returnConnection(connection);
            throw e;
        }
    }

    void AmqpConnectionPool::bindQueue(const AmqpQueueBinding &queue_binding) {
        auto connection = takeConnection();
        try {
            connection->bindQueue(queue_binding);
            returnConnection(connection);
        } catch (const amqp_exception &e) {
            returnConnection(connection);
            throw e;
        }
    }

    std::shared_ptr<AmqpConnection> AmqpConnectionPool::takeConnection() {
        std::lock_guard<std::mutex> guard(mtx);
        if (connections.empty()) {
            return std::make_shared<AmqpConnection>(xg::newGuid().str(), options);
        } else {
            auto conn = std::move(connections.front());
            connections.pop();
            return conn;
        }
    }

    void AmqpConnectionPool::returnConnection(std::shared_ptr<AmqpConnection> connection) {
        std::lock_guard<std::mutex> guard(mtx);
        connections.push(std::move(connection));
    }

    AmqpConnectionPool::AmqpConnectionPool(AmqpConnectionOpts options)
            : options(std::move(options)) {}
}