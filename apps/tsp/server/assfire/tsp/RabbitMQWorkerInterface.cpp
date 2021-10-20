#include "RabbitMQWorkerInterface.hpp"

namespace assfire::tsp::worker {
    const std::string RabbitMQWorkerInterface::TASK_QUEUE_NAME("org.assfire.tsp.worker.task");
    const std::string RabbitMQWorkerInterface::CONTROL_QUEUE_NAME("org.assfire.tsp.worker.signal");
    const std::string RabbitMQWorkerInterface::RESULT_QUEUE_NAME("org.assfire.tsp.worker.result");

    namespace {
        constexpr int AMQP_CHANNEL_ID = 1;

        const std::string TASK_EXCHANGE = "amq.direct";
        const std::string SIGNAL_EXCHANGE = "amq.topic";
    }

    using WorkerControlSignal = assfire::api::v1::tsp::WorkerControlSignal;

    RabbitMQWorkerInterface::RabbitMQWorkerInterface(const std::string &host, int port, const std::string &login, const std::string &password) {
        connection = amqp_new_connection();
        socket = amqp_tcp_socket_new(connection);

        if (!socket) {
            // [TODO]
        }

        int status = amqp_socket_open(socket, host.c_str(), port);
        if (status != 0) {
            // [TODO]
        }

        amqp_rpc_reply_t reply = amqp_login(connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, login.c_str(), password.c_str());

        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            // [TODO]
        }

        amqp_channel_open(connection, AMQP_CHANNEL_ID);

        reply = amqp_get_rpc_reply(connection);

        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            // [TODO]
        }
    }

    template<typename T>
    void publish(amqp_connection_state_t connection, const T &message, const std::string &queue_name, const std::string &exchange_name, std::mutex &lock) {
        std::lock_guard<std::mutex> guard(lock);
        std::size_t message_size = message.ByteSizeLong();
        void *buffer = malloc(message_size);
        message.SerializeToArray(buffer, message_size);
        amqp_bytes_t message_bytes;
        message_bytes.len = message_size;
        message_bytes.bytes = buffer;
        amqp_basic_publish(connection, AMQP_CHANNEL_ID, amqp_cstring_bytes(exchange_name.c_str()),
                           amqp_cstring_bytes(queue_name.c_str()), 1, 0,
                           nullptr, message_bytes);
        free(buffer);
    }

    void RabbitMQWorkerInterface::publishNewTask(const WorkerTask &task) {
        publish(connection, task, TASK_QUEUE_NAME, TASK_EXCHANGE, publish_lock);
    }

    void RabbitMQWorkerInterface::publishPauseEvent(const std::string &task_id) {
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_PAUSE);
        signal.set_task_id(task_id);

        publish(connection, signal, CONTROL_QUEUE_NAME, SIGNAL_EXCHANGE, publish_lock);
    }

    void RabbitMQWorkerInterface::publishStopEvent(const std::string &task_id) {
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT);
        signal.set_task_id(task_id);

        publish(connection, signal, CONTROL_QUEUE_NAME, SIGNAL_EXCHANGE, publish_lock);
    }

    void RabbitMQWorkerInterface::publishResumeEvent(const std::string &task_id) {
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_RESUME);
        signal.set_task_id(task_id);

        publish(connection, signal, CONTROL_QUEUE_NAME, SIGNAL_EXCHANGE, publish_lock);
    }
}
