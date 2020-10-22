#include "TspSolver.hpp"
#include "assfire/tsp/algorithms/two_opt/TwoOptTspAlgorithm.hpp"
#include "spdlog/spdlog.h"

using namespace assfire::tsp;

TspSolver::TspSolver(const router::RouterClient &router_client, const assfire::scheduler::transport::Scheduler &scheduler, const assfire::estimator::transport::ScheduleEstimator &estimator)
        : router_client(router_client), scheduler(scheduler), estimator(estimator)
{
}

TspSolver::Solution
TspSolver::solveTsp(const Task &task, const Settings &settings, const StatusConsumer &status_consumer,
                    const InterruptCondition &interruption_condition)
{
    switch (settings.algorithm()) {
        case api::v1::service::tsp::TSP_ALGORITHM_UNSPECIFIED:
        case api::v1::service::tsp::TSP_ALGORITHM_TWO_OPT: {
            TspSolutionStatus before_status;
            before_status.set_code(TspSolutionStatus::RESPONSE_STATUS_CODE_IN_PROGRESS);
            before_status.set_progress(0);
            status_consumer(before_status);

            try {
                TspSolver::Solution solution = TwoOptTspAlgorithm(router_client, scheduler, estimator).solveTsp(task, settings, status_consumer,
                                                                                          interruption_condition);
                TspSolutionStatus after_status;
                after_status.set_code(TspSolutionStatus::RESPONSE_STATUS_CODE_OK);
                after_status.set_progress(100);
                status_consumer(after_status);
                return solution;
            } catch (const std::exception& e) {
                SPDLOG_ERROR("Exception thrown while solving TSP: {}", e.what());
                TspSolutionStatus error_status;
                error_status.set_code(TspSolutionStatus::RESPONSE_STATUS_CODE_ERROR);
                error_status.set_progress(0);
                status_consumer(error_status);
                return TspSolver::Solution();
            }
        }
        default:
            SPDLOG_ERROR("Unknown TSP algorithm: {}", settings.algorithm());
            throw std::invalid_argument("Unknown TSP algorithm");
    }
}
