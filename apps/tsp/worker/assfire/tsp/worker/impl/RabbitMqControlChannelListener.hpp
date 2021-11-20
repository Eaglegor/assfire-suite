#pragma once

#include <mutex>
#include <functional>
#include <string>
#include <future>
#include "RabbitMqConnector.hpp"

namespace assfire::tsp {
    class RabbitMqControlChannelListener {
    public:
        using OnPauseListener = std::function<void(const std::string &)>;
        using OnInterruptListener = std::function<void(const std::string &)>;

        RabbitMqControlChannelListener(const std::string &host, int port, const std::string &login, const std::string &password);

        virtual ~RabbitMqControlChannelListener();

        void startListening();

        void setListeners(OnPauseListener on_pause_listener, OnInterruptListener on_interrupt_listener);

        void resetListeners();

    private:
        std::mutex mutex;
        std::future<void> control_state;
        OnPauseListener on_pause;
        OnInterruptListener on_interrupt;
        RabbitMqConnector rabbit_mq_connector;
    };
}