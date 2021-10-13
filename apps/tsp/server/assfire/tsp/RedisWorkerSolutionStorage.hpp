#pragma once

#include "WorkerSolutionStorage.hpp"

namespace assfire::tsp {
    class RedisWorkerSolutionStorage  : public WorkerSolutionStorage {
    public:
        std::optional<Solution> fetchSolution(const std::string &task_id) const override;

        std::unique_ptr<SolutionUpdatePublisher> subscribeForSolutionUpdate(const std::string &task_id) override;
    };
}