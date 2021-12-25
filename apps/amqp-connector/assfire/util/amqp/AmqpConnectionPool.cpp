#include "AmqpConnectionPool.hpp"
#include <crossguid/guid.hpp>

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

    std::shared_ptr<AmqpConnection> AmqpConnectionPool::takeConnection() {
        std::lock_guard<std::mutex> guard(mtx);
        if (connections.empty()) {
            auto conn = std::make_shared<AmqpConnection>(xg::newGuid().str(), options);
            conn->createConnection();
            return conn;
        } else {
            auto conn = std::move(connections.front());
            connections.pop();
            return conn;
        }
    }

    void AmqpConnectionPool::returnConnection(std::shared_ptr<AmqpConnection> connection) {
        std::lock_guard<std::mutex> guard(mtx);
        connections.push(connection);
    }
}