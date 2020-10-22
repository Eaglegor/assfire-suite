#pragma once

#include "assfire/tsp/TspAlgorithm.hpp"
#include "assfire/router/client/DistanceMatrix.hpp"
#include "assfire/estimator/transport/ScheduleEstimator.hpp"
#include <cstdint>

namespace assfire::router {
    class RouterClient;
}

namespace assfire::scheduler::transport {
    class Scheduler;
}

namespace assfire::tsp
{
    class Job;
    class Resource;
    class Solution;

    class TwoOptTspAlgorithm : public TspAlgorithm
    {
    public:
        using Cost = estimator::transport::ScheduleEstimator::Cost;

        TwoOptTspAlgorithm(const router::RouterClient &routing_client,
                           const scheduler::transport::Scheduler& scheduler,
                           const estimator::transport::ScheduleEstimator& estimator);

        TspSolution solveTsp(const Task &, const Settings &, const StatusConsumer &, const InterruptCondition&) override;

    private:
        Cost estimate(const Solution&) const;

        const router::RouterClient& router_client;
        const scheduler::transport::Scheduler& scheduler;
        const estimator::transport::ScheduleEstimator& estimator;
    };
}