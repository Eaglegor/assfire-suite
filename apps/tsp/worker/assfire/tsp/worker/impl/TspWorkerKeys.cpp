#include "TspWorkerKeys.hpp"
#include "TspWorkerConstants.hpp"

namespace assfire::tsp {
    std::string taskKey(const std::string &task_id) {
        return TSP_WORKER_REDIS_PREFIX + task_id + TSP_WORKER_TASK_SUFFIX;
    }

    std::string attemptsKey(const std::string &task_id) {
        return TSP_WORKER_REDIS_PREFIX + task_id + TSP_WORKER_ATTEMPTS_SUFFIX;
    }

    std::string lockKey(const std::string &task_id) {
        return TSP_WORKER_REDIS_PREFIX + task_id + TSP_WORKER_LOCK_SUFFIX;
    }

    std::string stateKey(const std::string &task_id) {
        return TSP_WORKER_REDIS_PREFIX + task_id + TSP_WORKER_STATE_SUFFIX;
    }

    std::string statusKey(const std::string &task_id) {
        return TSP_WORKER_REDIS_PREFIX + task_id + TSP_WORKER_STATUS_SUFFIX;
    }

    std::string keepAliveKey(const std::string &task_id) {
        return TSP_WORKER_REDIS_PREFIX + task_id + TSP_WORKER_KEEPALIVE_SUFFIX;
    }

    std::string solutionKey(const std::string &task_id) {
        return TSP_WORKER_REDIS_PREFIX + task_id + TSP_WORKER_SOLUTION_SUFFIX;
    }

    std::string statusQueueName(const std::string &task_id) {
        return std::string(TSP_WORKER_AMQP_PREFIX) + task_id + TSP_WORKER_AMQP_STATUS_SUFFIX;
    }
}