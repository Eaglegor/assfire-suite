#pragma once

#include <mutex>
#include <future>

#include "assfire/tsp/worker/InterruptListener.hpp"
#include "RabbitMqConnector.hpp"
#include "assfire/util/amqp/AmqpConnectionPool.hpp"
#include <atomic>

namespace assfire::tsp {
    class AmqpInterruptListener : public InterruptListener {
    public:
        AmqpInterruptListener(std::string name, util::AmqpConnectionPool& amqp_connection_pool);
        ~AmqpInterruptListener();

        void startListening() override;
        void stopListening() override;

        void subscribe(const std::string &task_id, SignalProcessor processor) override;
        void unsubscribe(const std::string &task_id) override;

    private:
        std::string name;
        std::mutex processors_lock;
        std::unordered_map<std::string, SignalProcessor> processors;
        std::atomic_bool interrupted;
        std::future<void> control_state;
        util::AmqpConnectionPool& connection_pool;
    };
}
