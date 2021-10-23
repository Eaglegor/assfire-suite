#include "RabbitMQWorkerTransport.hpp"
#include <spdlog/spdlog.h>

namespace assfire::tsp::worker {
    const std::string RabbitMQWorkerTransport::TASK_QUEUE_NAME("org.assfire.tsp.worker.task");
    const std::string RabbitMQWorkerTransport::CONTROL_QUEUE_NAME("org.assfire.tsp.worker.signal");
    const std::string RabbitMQWorkerTransport::RESULT_QUEUE_NAME("org.assfire.tsp.worker.result");

    namespace {
        constexpr int AMQP_CHANNEL_ID = 1;

        const std::string TASK_EXCHANGE = "amq.direct";
        const std::string SIGNAL_EXCHANGE = "amq.topic";
    }

    using WorkerControlSignal = assfire::api::v1::tsp::WorkerControlSignal;

    void processAmqpError(const amqp_rpc_reply_t &reply, const char *message, bool throw_error = true) {
        switch (reply.reply_type) {
            case AMQP_RESPONSE_NONE: {
                SPDLOG_ERROR("{}: {}", message, "Missing AMQP reply type");
                break;
            }

            case AMQP_RESPONSE_LIBRARY_EXCEPTION: {
                SPDLOG_ERROR("{}: {}", message, amqp_error_string2(reply.library_error));
                break;
            }

            case AMQP_RESPONSE_SERVER_EXCEPTION: {
                switch (reply.reply.id) {
                    case AMQP_CONNECTION_CLOSE_METHOD: {
                        amqp_connection_close_t *m =
                                (amqp_connection_close_t *) reply.reply.decoded;
                        SPDLOG_ERROR("{}: error {} ({})", message, m->reply_code, (const char *) m->reply_text.bytes);
                        break;
                    }
                    case AMQP_CHANNEL_CLOSE_METHOD: {
                        amqp_channel_close_t *m = (amqp_channel_close_t *) reply.reply.decoded;
                        SPDLOG_ERROR("{}: error {} ({})", message, m->reply_code, (const char *) m->reply_text.bytes);
                        break;
                    }
                    default:
                        amqp_channel_close_t *m = (amqp_channel_close_t *) reply.reply.decoded;
                        SPDLOG_ERROR("{}: unknown AMQP server error", message);
                }
                break;
            }
        }
        if (throw_error) throw std::runtime_error(message);
    }

    RabbitMQWorkerTransport::RabbitMQWorkerTransport(const std::string &host, int port, const std::string &login, const std::string &password) {
        SPDLOG_INFO("Initializing RabbitMQ worker transport");

        SPDLOG_INFO("Creating RabbitMQ socket");
        connection = amqp_new_connection();
        socket = amqp_tcp_socket_new(connection);

        try {
            if (!socket) {
                SPDLOG_ERROR("Failed to create RabbitMQ socket");
                throw std::runtime_error("Failed to create RabbitMQ socket");
            }

            SPDLOG_INFO("Connecting to RabbitMQ at {}:{}", host, port);
            int status = amqp_socket_open(socket, host.c_str(), port);
            if (status != AMQP_STATUS_OK) {
                SPDLOG_ERROR("Failed to connect to RabbitMQ: {}", amqp_error_string2(status));
                throw std::runtime_error("Failed to connect to RabbitMQ");
            }

            SPDLOG_INFO("Logging in to RabbitMQ as {}...", login.c_str());
            amqp_rpc_reply_t reply = amqp_login(connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, login.c_str(), password.c_str());

            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                SPDLOG_ERROR("Failed to log in to RabbitMQ: {}", amqp_error_string2(reply.library_error));
                throw std::runtime_error("Failed to log in to RabbitMQ");
            }

            SPDLOG_INFO("Opening RabbitMQ channel #{}...", AMQP_CHANNEL_ID);
            amqp_channel_open(connection, AMQP_CHANNEL_ID);
            reply = amqp_get_rpc_reply(connection);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                SPDLOG_ERROR("Failed to open RabbitMQ channel: {}", amqp_error_string2(reply.library_error));
                throw std::runtime_error("Failed to open RabbitMQ channel");
            }

            SPDLOG_INFO("Declaring RabbitMQ tasks queue {}", TASK_QUEUE_NAME);
            amqp_queue_declare(connection, AMQP_CHANNEL_ID, amqp_cstring_bytes(TASK_QUEUE_NAME.c_str()),
                               0,0,0,1,amqp_empty_table);
            reply = amqp_get_rpc_reply(connection);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                processAmqpError(reply, "Failed to declare RabbitMQ queue");
            }

            amqp_queue_bind(connection, AMQP_CHANNEL_ID, amqp_cstring_bytes(TASK_QUEUE_NAME.c_str()), amqp_cstring_bytes(TASK_EXCHANGE.c_str()),
                            amqp_cstring_bytes(TASK_QUEUE_NAME.c_str()), amqp_empty_table);
            reply = amqp_get_rpc_reply(connection);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                processAmqpError(reply, "Failed to bind RabbitMQ queue");
            }
        } catch (const std::runtime_error& e) {
            SPDLOG_ERROR("Failed to initialize RabbitMQ transport. Cleaning up connections...");
            amqp_connection_close(connection, AMQP_REPLY_SUCCESS);
            amqp_destroy_connection(connection);
            SPDLOG_ERROR("RabbitMQ cleanup finished");
            throw e;
        }
        SPDLOG_INFO("RabbitMQ worker transport successfully initialized");
    }

    RabbitMQWorkerTransport::~RabbitMQWorkerTransport() {
        SPDLOG_INFO("Destroying RabbitMQ worker transport");

        SPDLOG_INFO("Closing RabbitMQ channel #{}...", AMQP_CHANNEL_ID);
        amqp_rpc_reply_t reply = amqp_channel_close(connection, AMQP_CHANNEL_ID, AMQP_REPLY_SUCCESS);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            SPDLOG_ERROR("Failed to close RabbitMQ channel {}: {}", AMQP_CHANNEL_ID, amqp_error_string2(reply.library_error));
        }

        SPDLOG_INFO("Closing RabbitMQ connection...");
        reply = amqp_connection_close(connection, AMQP_REPLY_SUCCESS);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            SPDLOG_ERROR("Failed to close RabbitMQ connection: {}", amqp_error_string2(reply.library_error));
        }

        SPDLOG_INFO("Destroying RabbitMQ connection...");
        int status = amqp_destroy_connection(connection);
        if (status != AMQP_STATUS_OK) {
            SPDLOG_ERROR("Failed to destroy RabbitMQ connection: {}", amqp_error_string2(status));
        }

        SPDLOG_INFO("RabbitMQ worker transport destroyed");
    }

    template<typename T>
    void publish(amqp_connection_state_t connection, const T &message, const std::string &queue_name, const std::string &exchange_name, std::mutex &lock) {
        SPDLOG_TRACE("Obtaning publish lock");
        std::lock_guard<std::mutex> guard(lock);
        SPDLOG_TRACE("Publish lock obtained");
        std::size_t message_size = message.ByteSizeLong();
        SPDLOG_TRACE("Allocating {} bytes for proto message", message_size);
        void *buffer = malloc(message_size);
        SPDLOG_TRACE("Serializing message");
        message.SerializeToArray(buffer, message_size);
        amqp_bytes_t message_bytes;
        message_bytes.len = message_size;
        message_bytes.bytes = buffer;
        SPDLOG_DEBUG("Sending {} bytes to RabbitMQ", message_bytes.len);
        auto status = amqp_basic_publish(connection, AMQP_CHANNEL_ID, amqp_cstring_bytes(exchange_name.c_str()),
                           amqp_cstring_bytes(queue_name.c_str()), 1, 0,
                           nullptr, message_bytes);
        if(status != AMQP_STATUS_OK) {
            SPDLOG_ERROR("Failed to send task to RabbitMQ: {}", amqp_error_string2(status));
            throw std::runtime_error("Failed to send task to RabbitMQ");
        }
        SPDLOG_TRACE("Cleaning up memory", message_size);
        free(buffer);
    }

    void RabbitMQWorkerTransport::publishNewTask(const WorkerTask &task) {
        SPDLOG_INFO("Publishing worker task {} to RabbitMQ", task.task_id());
        publish(connection, task, TASK_QUEUE_NAME, TASK_EXCHANGE, publish_lock);
    }

    void RabbitMQWorkerTransport::publishPauseEvent(const std::string &task_id) {
        SPDLOG_INFO("Publishing pause signal for task {} to RabbitMQ", task_id);
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_PAUSE);
        signal.set_task_id(task_id);

        publish(connection, signal, CONTROL_QUEUE_NAME, SIGNAL_EXCHANGE, publish_lock);
    }

    void RabbitMQWorkerTransport::publishStopEvent(const std::string &task_id) {
        SPDLOG_INFO("Publishing stop signal for task {} to RabbitMQ", task_id);
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_INTERRUPT);
        signal.set_task_id(task_id);

        publish(connection, signal, CONTROL_QUEUE_NAME, SIGNAL_EXCHANGE, publish_lock);
    }

    void RabbitMQWorkerTransport::publishResumeEvent(const std::string &task_id) {
        SPDLOG_INFO("Publishing resume signal for task {} to RabbitMQ", task_id);
        WorkerControlSignal signal;
        signal.set_signal_type(WorkerControlSignal::WORKER_CONTROL_SIGNAL_TYPE_RESUME);
        signal.set_task_id(task_id);

        publish(connection, signal, CONTROL_QUEUE_NAME, SIGNAL_EXCHANGE, publish_lock);
    }
}
