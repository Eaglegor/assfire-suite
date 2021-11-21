#pragma once

#include <string>

namespace assfire::tsp {
    std::string taskKey(const std::string &task_id);

    std::string attemptsKey(const std::string &task_id);

    std::string lockKey(const std::string &task_id);

    std::string stateKey(const std::string &task_id);

    std::string statusKey(const std::string &task_id);

    std::string keepAliveKey(const std::string &task_id);

    std::string solutionKey(const std::string &task_id);
}