#pragma once

#include <string>
#include <memory>
#include "assfire/tsp/worker/SolutionPublisher.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {
    class RedisSolutionPublisher : public SolutionPublisher {
    public:
        RedisSolutionPublisher(std::unique_ptr<cpp_redis::client> client);

        ~RedisSolutionPublisher();

        void publish(const std::string &task_id, const TspTask &task, const TspSolution &solution) override;

    private:
        std::unique_ptr<cpp_redis::client> client;
    };
}