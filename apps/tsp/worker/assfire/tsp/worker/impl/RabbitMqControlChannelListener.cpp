#include "RabbitMqControlChannelListener.hpp"
#include <spdlog/spdlog.h>
#include <assfire/api/v1/tsp/worker.pb.h>
#include "TspImplConstants.hpp"

namespace assfire::tsp {
    RabbitMqControlChannelListener::RabbitMqControlChannelListener(const std::string &host, int port, const std::string &login, const std::string &password) :
            rabbit_mq_connector("ControlChannelListener") {
        SPDLOG_INFO("Initializing RabbitMQ control channel listener");

        rabbit_mq_connector.connect(host, port, login, password);

        resetListeners();
        SPDLOG_INFO("RabbitMQ control channel listener initialized");
    }

    void RabbitMqControlChannelListener::startListening() {
        SPDLOG_INFO("Starting RabbitMQ control signals processing");

        control_state = std::async(std::launch::async, [&]() {
            rabbit_mq_connector.listen(
                    TSP_AMQP_WORKER_CONTROL_SIGNAL_QUEUE_NAME,
                    TSP_AMQP_WORKER_CONTROL_SIGNAL_EXCHANGE_NAME,
                    TSP_AMQP_WORKER_CONTROL_SIGNAL_CHANNEL_ID,
                    [&](const amqp_envelope_t_ &envelope) {
                        assfire::api::v1::tsp::WorkerControlSignal worker_signal;
                        worker_signal.ParseFromArray(envelope.message.body.bytes, envelope.message.body.len);

                        {
                            std::lock_guard<std::mutex> guard(mutex);
                            switch (worker_signal.signal_type()) {
                                case assfire::api::v1::tsp::WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_PAUSE:
                                    on_pause(worker_signal.task_id());
                                    break;
                                case assfire::api::v1::tsp::WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT:
                                    on_interrupt(worker_signal.task_id());
                                    break;
                                default:
                                    SPDLOG_WARN("Unknown control signal received for task {}: {}", worker_signal.task_id(), static_cast<int>(worker_signal.signal_type()));
                            }
                        }
                    }
            );
        });
    }

    void RabbitMqControlChannelListener::setListeners(RabbitMqControlChannelListener::OnPauseListener on_pause_listener,
                                                      RabbitMqControlChannelListener::OnInterruptListener on_interrupt_listener) {
        std::lock_guard<std::mutex> guard(mutex);
        this->on_pause = std::move(on_pause_listener);
        this->on_interrupt = std::move(on_interrupt_listener);
    }

    void RabbitMqControlChannelListener::resetListeners() {
        std::lock_guard<std::mutex> guard(mutex);
        this->on_pause = [](const std::string &) {};
        this->on_interrupt = [](const std::string &) {};
    }

    RabbitMqControlChannelListener::~RabbitMqControlChannelListener() {
        rabbit_mq_connector.interrupt();
        control_state.wait();
    }
}