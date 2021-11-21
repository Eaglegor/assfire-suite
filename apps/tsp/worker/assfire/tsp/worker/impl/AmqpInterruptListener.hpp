#pragma once

#include <mutex>
#include <future>

#include "assfire/tsp/worker/InterruptListener.hpp"
#include "RabbitMqConnector.hpp"

namespace assfire::tsp {
    class AmqpInterruptListener : public InterruptListener {
    public:
        AmqpInterruptListener(std::unique_ptr<RabbitMqConnector> rabbit_mq_connector);
        ~AmqpInterruptListener();

        void startListening() override;

        void subscribe(const std::string &task_id, SignalProcessor processor) override;
        void unsubscribe(const std::string &task_id) override;

    private:
        std::mutex processors_lock;
        std::unordered_map<std::string, SignalProcessor> processors;
        std::unique_ptr<RabbitMqConnector> rabbit_mq_connector;
        std::future<void> control_state;
    };
}
