#include "RabbitMQWorkerInterface.hpp"

namespace assfire::tsp::worker {
    const std::string RabbitMQWorkerInterface::TASK_QUEUE_NAME("WORKER_TASK_QUEUE");
    const std::string RabbitMQWorkerInterface::CONTROL_QUEUE_NAME("CONTROL_QUEUE");
    const std::string RabbitMQWorkerInterface::RESULT_QUEUE_NAME("RESULT_QUEUE");

    using WorkerControlSignal = assfire::api::v1::tsp::WorkerControlSignal;

    RabbitMQWorkerInterface::RabbitMQWorkerInterface() :
            connection(nullptr),
            channel(&connection) {
        channel.declareQueue(TASK_QUEUE_NAME);
        channel.declareQueue(CONTROL_QUEUE_NAME);
        channel.declareQueue(RESULT_QUEUE_NAME);
    }

    void RabbitMQWorkerInterface::publishNewTask(const WorkerTask &task) {
        std::string message = task.SerializeAsString();
        channel.publish(TASK_QUEUE_NAME, "my-key", message.c_str());
    }

    void RabbitMQWorkerInterface::publishPauseEvent(const std::string &task_id) {
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_PAUSE);
        signal.set_task_id(task_id);
        std::string message = signal.SerializeAsString();
        channel.publish(CONTROL_QUEUE_NAME, "my-key", message.c_str());
    }

    void RabbitMQWorkerInterface::publishStopEvent(const std::string &task_id) {
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT);
        signal.set_task_id(task_id);
        std::string message = signal.SerializeAsString();
        channel.publish(CONTROL_QUEUE_NAME, "my-key", message.c_str());
    }

    void RabbitMQWorkerInterface::publishResumeEvent(const std::string &task_id) {
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_RESUME);
        signal.set_task_id(task_id);
        std::string message = signal.SerializeAsString();
        channel.publish(CONTROL_QUEUE_NAME, "my-key", message.c_str());
    }
}
