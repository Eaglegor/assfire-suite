#include "AmqpInterruptListener.hpp"
#include <assfire/api/v1/tsp/worker.pb.h>
#include <spdlog/spdlog.h>

namespace assfire::tsp {

    AmqpInterruptListener::AmqpInterruptListener(std::unique_ptr<RabbitMqConnector> rabbit_mq_connector)
            : rabbit_mq_connector(std::move(rabbit_mq_connector)) {}

    void AmqpInterruptListener::startListening() {
        control_state = std::async(std::launch::async, [&]() {
            rabbit_mq_connector->listen(
                    "org.assfire.tsp.worker.signal",
                    "amq.topic",
                    0,
                    [&](const amqp_envelope_t_ &envelope) {
                        assfire::api::v1::tsp::WorkerControlSignal worker_signal;
                        worker_signal.ParseFromArray(envelope.message.body.bytes, envelope.message.body.len);

                        SignalProcessor process;
                        {
                            std::lock_guard<std::mutex> guard(processors_lock);
                            auto processor_iter = processors.find(worker_signal.task_id());
                            if(processor_iter == processors.end()) return;
                            process = processor_iter->second;
                        }

                        switch (worker_signal.signal_type()) {
                            case assfire::api::v1::tsp::WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_PAUSE:
                                process(InterruptListener::PAUSE);
                                break;
                            case assfire::api::v1::tsp::WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT:
                                process(InterruptListener::INTERRUPT);
                                break;
                            default:
                                SPDLOG_WARN("Unknown control signal received for task {}: {}", worker_signal.task_id(), static_cast<int>(worker_signal.signal_type()));
                        }
                    }
                    );
        });
    }

    void AmqpInterruptListener::subscribe(const std::string &task_id, InterruptListener::SignalProcessor processor) {
        std::lock_guard<std::mutex> lock(processors_lock);
        processors.emplace(task_id, processor);
    }

    void AmqpInterruptListener::unsubscribe(const std::string &task_id) {
        std::lock_guard<std::mutex> lock(processors_lock);
        processors.erase(task_id);
    }

    AmqpInterruptListener::~AmqpInterruptListener() {
        control_state.wait();
    }
}