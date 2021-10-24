#pragma once

#include "SolutionStorage.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {
    class RedisSolutionStorage  : public SolutionStorage {
    public:
        std::optional<Solution> fetchSolution(const std::string &task_id) const override;

    private:
        std::shared_ptr<cpp_redis::client> redis_client;
    };
}