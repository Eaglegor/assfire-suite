#pragma once

namespace assfire::tsp {
    constexpr const char* TSP_REDIS_WORKER_KEY_PREFIX = "tsp:";
    constexpr const char* TSP_REDIS_WORKER_SOLUTION_KEY_SUFFIX = ":solution";
    constexpr const char* TSP_REDIS_WORKER_TASK_KEY_SUFFIX = ":task";
    constexpr const char* TSP_REDIS_WORKER_ATTEMPTS_COUNTER_KEY_SUFFIX = ":attempts.count";
    constexpr const char* TSP_REDIS_WORKER_TASK_PROGRESS_MARKER_SUFFIX = ":in.progress";


    constexpr const char* TSP_AMQP_WORKER_TASK_QUEUE_NAME = "org.assfire.tsp.worker.task";
    constexpr const char* TSP_AMQP_WORKER_CONTROL_SIGNAL_QUEUE_NAME = "org.assfire.tsp.worker.control_signal";
    constexpr const char* TSP_AMQP_WORKER_STATUS_UPDATE_QUEUE_NAME = "org.assfire.tsp.worker.status";
    constexpr const char* TSP_AMQP_WORKER_TASK_EXCHANGE_NAME = "amq.direct";
    constexpr const char* TSP_AMQP_WORKER_CONTROL_SIGNAL_EXCHANGE_NAME = "amq.topic";
    constexpr const char* TSP_AMQP_WORKER_STATUS_UPDATE_EXCHANGE_NAME = "amq.topic";

    constexpr int TSP_AMQP_WORKER_TASK_CHANNEL_ID = 1;
    constexpr int TSP_AMQP_WORKER_CONTROL_SIGNAL_CHANNEL_ID = 2;
    constexpr int TSP_AMQP_WORKER_STATUS_UPDATE_CHANNEL_ID = 3;

    constexpr int TSP_AMQP_TASK_QUEUE_CHANNEL_ID = 1;
    constexpr int TSP_AMQP_STATUS_UPDATE_CHANNEL_ID = 2;
    constexpr int TSP_AMQP_INTERRUPT_SIGNALS_CHANNEL_ID = 3;

    constexpr int TSP_MAX_ATTEMPTS = 5;
}