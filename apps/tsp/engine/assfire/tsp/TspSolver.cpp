#include "TspSolver.hpp"
#include "assfire/tsp/algorithms/two_opt/TwoOptTspAlgorithm.hpp"
#include "spdlog/spdlog.h"

using namespace assfire::tsp;

TspSolver::TspSolver(const router::RouterClient &router_client)
        : router_client(router_client)
{
}

TspSolver::Solution
TspSolver::solveTsp(const Task &task, const Settings &settings, const StatusConsumer &status_consumer,
                    const InterruptCondition &interruption_condition)
{
    switch (settings.algorithm()) {
        case Settings::TWO_OPT: {
            TspSolutionStatus before_status;
            before_status.set_code(TspSolutionStatus::IN_PROGRESS);
            before_status.set_progress(0);
            status_consumer(before_status);

            try {
                TspSolver::Solution solution = TwoOptTspAlgorithm(router_client).solveTsp(task, settings, status_consumer,
                                                                                          interruption_condition);
                TspSolutionStatus after_status;
                after_status.set_code(TspSolutionStatus::OK);
                after_status.set_progress(100);
                status_consumer(after_status);
                return solution;
            } catch (const std::exception& e) {
                SPDLOG_ERROR("Exception thrown while solving TSP: {}", e.what());
                TspSolutionStatus error_status;
                error_status.set_code(TspSolutionStatus::ERROR);
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
