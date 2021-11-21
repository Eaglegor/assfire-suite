#pragma once

namespace assfire::tsp {
    constexpr const char *TSP_WORKER_REDIS_PREFIX = "assfire.tsp.";

    constexpr const char *TSP_WORKER_TASK_SUFFIX = ".task";
    constexpr const char *TSP_WORKER_ATTEMPTS_SUFFIX = ".attempts";
    constexpr const char *TSP_WORKER_LOCK_SUFFIX = ".lock";
    constexpr const char *TSP_WORKER_STATUS_SUFFIX = ".status";
    constexpr const char *TSP_WORKER_KEEPALIVE_SUFFIX = ".keepAlive";
    constexpr const char *TSP_WORKER_SOLUTION_SUFFIX = ".solution";
    constexpr const char *TSP_WORKER_STATE_SUFFIX = ".state";

    constexpr const char *TSP_WORKER_LOCK_VALUE = "1";
    constexpr const char *TSP_WORKER_KEEPALIVE_VALUE = "1";
    constexpr int TSP_WORKER_KEEPALIVE_EXPIRY_SEC = 30;

    constexpr const char *TSP_WORKER_AMQP_PREFIX = "assfire.tsp.";
    constexpr const char *TSP_WORKER_AMQP_STATUS_SUFFIX = ".worker.status";
    constexpr const char *TSP_WORKER_AMQP_TASK_QUEUE_NAME = "assfire.tsp.worker.task";
    constexpr const char *TSP_WORKER_AMQP_INTERRUPT_QUEUE_NAME = "assfire.tsp.worker.interrupt";

    constexpr const char *TSP_WORKER_AMQP_TASK_EXCHANGE = "amq.direct";
    constexpr const char *TSP_WORKER_AMQP_INTERRUPT_EXCHANGE = "amq.topic";
    constexpr const char *TSP_WORKER_AMQP_STATUS_EXCHANGE = "amq.topic";

    constexpr int TSP_WORKER_AMQP_TASK_CHANNEL = 1;
    constexpr int TSP_WORKER_AMQP_INTERRUPT_CHANNEL = 2;
    constexpr int TSP_WORKER_AMQP_STATUS_CHANNEL = 3;

    constexpr int TSP_WORKER_MAX_ATTEMPTS = 5;


}