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
    class Solution;

    class TwoOptTspAlgorithm : public TspAlgorithm
    {
    public:
        using Cost = std::int64_t;

        TwoOptTspAlgorithm(const router::RouterClient &routing_client);

        TspSolution solveTsp(const Task &, const Settings &, const StatusConsumer &, const InterruptCondition&) override;

    private:
        static Cost estimate(const Solution&);

        const router::RouterClient& router_client;
    };
}