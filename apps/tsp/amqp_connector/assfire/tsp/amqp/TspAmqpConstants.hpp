#pragma once

namespace assfire::tsp {
    constexpr const char* TSP_AMQP_WORKER_TASK_QUEUE_NAME = "org.assfire.tsp.worker.task";
    constexpr const char* TSP_AMQP_WORKER_CONTROL_SIGNAL_QUEUE_NAME = "org.assfire.tsp.worker.control_signal";
    constexpr const char* TSP_AMQP_WORKER_STATUS_UPDATE_QUEUE_NAME = "org.assfire.tsp.worker.status";
    constexpr const char* TSP_AMQP_WORKER_TASK_EXCHANGE_NAME = "amq.direct";
    constexpr const char* TSP_AMQP_WORKER_CONTROL_SIGNAL_EXCHANGE_NAME = "amq.topic";
    constexpr const char* TSP_AMQP_WORKER_STATUS_UPDATE_EXCHANGE_NAME = "amq.topic";
    constexpr int TSP_AMQP_WORKER_TASK_CHANNEL_ID = 1;
    constexpr int TSP_AMQP_WORKER_CONTROL_SIGNAL_CHANNEL_ID = 2;
    constexpr int TSP_AMQP_WORKER_STATUS_UPDATE_CHANNEL_ID = 3;
}