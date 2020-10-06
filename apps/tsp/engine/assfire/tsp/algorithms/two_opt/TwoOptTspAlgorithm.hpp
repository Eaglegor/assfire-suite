#pragma once

#include "assfire/tsp/TspAlgorithm.hpp"
#include "assfire/router/client/DistanceMatrix.hpp"
#include <cstdint>

namespace assfire::router {
    class RouterClient;
}

namespace assfire::tsp
{
    class Job;
    class Resource;

    class TwoOptTspAlgorithm : public TspAlgorithm
    {
    public:
        TwoOptTspAlgorithm(const router::RouterClient &routing_client);

        Solution solveTsp(const Task &, const Settings &, const StatusConsumer &, const InterruptCondition&) override;

    private:
        static std::int64_t estimate(const std::vector<Job>&, const Resource&);

        const router::RouterClient& router_client;
    };
}