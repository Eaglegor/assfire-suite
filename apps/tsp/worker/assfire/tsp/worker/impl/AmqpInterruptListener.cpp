#include "AmqpInterruptListener.hpp"
#include <chrono>
#include <assfire/api/v1/tsp/worker.pb.h>
#include <spdlog/spdlog.h>
#include "TspWorkerConstants.hpp"
#include "TspWorkerKeys.hpp"
#include "assfire/util/amqp/AmqpProtoParse.hpp"

namespace assfire::tsp
{

    using WorkerSignal = assfire::api::v1::tsp::WorkerControlSignal;
    using namespace std::literals::chrono_literals;

    AmqpInterruptListener::AmqpInterruptListener(std::string name, util::AmqpConnectionPool &connection_pool)
            : name(name), connection_pool(connection_pool) {}

    void AmqpInterruptListener::startListening() {
        control_state = std::async(std::launch::async, [&]() {

            std::string queue_name = connection_pool.declareQueue(
                    name + "/queue",
                    {
                            "",
                            false,
                            false,
                            false,
                            true
                    }
            );

            connection_pool.bindQueue({
                                              queue_name,
                                              TSP_WORKER_AMQP_INTERRUPT_EXCHANGE,
                                              ""
                                      });

            util::AmqpConnectionPool::ConsumerRef consumer = connection_pool.createConsumer(name, {
                    {
                            queue_name,
                            false,
                            false,
                            false
                    },
                    true
            });

            while (!interrupted) {
                consumer->consumeMessage([&](util::AmqpDelivery &delivery) {
                    auto signal = delivery.parse<WorkerSignal>(util::AmqpProtoParse<WorkerSignal>());
                    SignalProcessor process;
                    {
                        std::lock_guard<std::mutex> guard(processors_lock);
                        auto processor_iter = processors.find(signal.task_id());
                        if (processor_iter == processors.end()) return;
                        process = processor_iter->second;
                    }

                    switch (signal.signal_type()) {
                        case assfire::api::v1::tsp::WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_PAUSE:
                            process(InterruptListener::PAUSE);
                            break;
                        case assfire::api::v1::tsp::WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT:
                            process(InterruptListener::INTERRUPT);
                            break;
                        default:
                            SPDLOG_WARN("Unknown control signal received for task {}: {}", signal.task_id(), static_cast<int>(signal.signal_type()));
                    }

                    delivery.ack();
                }, 5000ms);
            }
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
        if (control_state.valid()) {
            control_state.wait();
        }
    }

    void AmqpInterruptListener::stopListening() {
        interrupted = true;
    }
}