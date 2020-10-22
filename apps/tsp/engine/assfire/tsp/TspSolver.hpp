#pragma once

#include <assfire/api/v1/service/tsp/tsp.pb.h>
#include "TspAlgorithm.hpp"

namespace assfire::router {
    class RouterClient;
}

namespace assfire::scheduler::transport {
    class Scheduler;
}

namespace assfire::estimator::transport {
    class ScheduleEstimator;
}

namespace assfire::tsp
{
    namespace {
        const TspAlgorithm::StatusConsumer DEFAULT_STATUS_CONSUMER = [](const TspAlgorithm::TspSolutionStatus&){};
        const TspAlgorithm::InterruptCondition DEFAULT_INTERRUPT_CONDITION = [](){return false;};
    }

    class TspSolver
    {
    public:
        using Task = TspAlgorithm::Task;
        using Solution = TspAlgorithm::TspSolution;
        using Settings = TspAlgorithm::Settings;
        using TspSolutionStatus = TspAlgorithm::TspSolutionStatus;

        using StatusConsumer = TspAlgorithm::StatusConsumer;
        using InterruptCondition = TspAlgorithm::InterruptCondition;

        TspSolver(const router::RouterClient &router_client, const assfire::scheduler::transport::Scheduler &scheduler, const assfire::estimator::transport::ScheduleEstimator &estimator);

        Solution solveTsp(const Task&, const Settings&, const StatusConsumer& status_consumer = DEFAULT_STATUS_CONSUMER, const InterruptCondition& interrupt_condition = DEFAULT_INTERRUPT_CONDITION);

        virtual ~TspSolver() = default;

    private:
        const router::RouterClient& router_client;
        const scheduler::transport::Scheduler& scheduler;
        const estimator::transport::ScheduleEstimator& estimator;
    };
}