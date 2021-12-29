#pragma once

#include <string>
#include <memory>
#include "assfire/tsp/worker/SolutionPublisher.hpp"
#include "assfire/util/redis/RedisConnector.hpp"

namespace assfire::tsp {
    class RedisSolutionPublisher : public SolutionPublisher {
    public:
        RedisSolutionPublisher(util::RedisConnector& redis_connector);

        void publish(const std::string &task_id, const TspTask &task, const TspSolution &solution) override;

    private:
        util::RedisConnector& redis_connector;
    };
}