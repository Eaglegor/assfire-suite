#pragma once

#include <string>
#include <memory>
#include "SolutionPublisher.hpp"

namespace cpp_redis {
    class client;
}

namespace assfire::tsp {
    class RedisSolutionPublisher : public SolutionPublisher {
    public:
        RedisSolutionPublisher(const std::string& redis_host, std::size_t redis_port);

        void publish(const std::string &task_id, const TspTask &task, const TspSolution &solution) override;

        void onError(const std::string &task_id) override;

    private:
        std::unique_ptr<cpp_redis::client> client;
    };
}