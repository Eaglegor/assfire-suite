#pragma once

namespace assfire::tsp {
    constexpr const char* TSP_REDIS_WORKER_KEY_PREFIX = "tsp:";
    constexpr const char* TSP_REDIS_WORKER_SOLUTION_KEY_SUFFIX = ":solution";
    constexpr const char* TSP_REDIS_WORKER_TASK_KEY_SUFFIX = ":task";
    constexpr const char* TSP_REDIS_WORKER_ATTEMPTS_COUNTER_KEY_SUFFIX = ":attempts.count";
    constexpr const char* TSP_REDIS_WORKER_TASK_PROGRESS_MARKER_SUFFIX = ":in.progress";
}