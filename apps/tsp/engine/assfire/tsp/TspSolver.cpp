#include "TspSolver.hpp"
#include "assfire/tsp/algorithms/two_opt/TwoOptTspAlgorithm.hpp"
#include "spdlog/spdlog.h"

using namespace assfire::tsp;

TspSolver::Solution TspSolver::solveTsp(const Task &task, const Settings &settings, const StatusConsumer &status_consumer, const InterruptCondition& interruption_condition)
{
    switch (settings.algorithm()) {
        case Settings::TWO_OPT: {
            TspSolver::Solution solution = TwoOptTspAlgorithm().solveTsp(task, settings, status_consumer, interruption_condition);
            TspSolutionStatus status;
            status.set_finished(true);
            status.set_progress(100);
            status_consumer(status);
            return solution;
        }
        default:
            SPDLOG_ERROR("Unknown TSP algorithm: {}", settings.algorithm());
            throw std::invalid_argument("Unknown TSP algorithm");
    }
}
