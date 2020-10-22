#include "TwoOptTspAlgorithm.hpp"
#include "assfire/router/client/RouterClient.hpp"
#include "assfire/tsp/algorithms/model/Problem.hpp"
#include "assfire/tsp/algorithms/model/Solution.hpp"
#include "assfire/scheduler/transport/Scheduler.hpp"
#include "spdlog/spdlog.h"
#include <vector>

using namespace assfire::tsp;
using namespace assfire::router;
using namespace assfire::scheduler::transport;
using namespace assfire::estimator::transport;

TwoOptTspAlgorithm::TwoOptTspAlgorithm(const RouterClient &routing_client, const Scheduler &scheduler, const ScheduleEstimator &estimator)
        : router_client(routing_client), scheduler(scheduler), estimator(estimator)
{

}

TspAlgorithm::TspSolution
TwoOptTspAlgorithm::solveTsp(const TspAlgorithm::Task &task, const TspAlgorithm::Settings &settings,
                             const TspAlgorithm::StatusConsumer &consumer,
                             const InterruptCondition &interruptCondition)
{
    Problem problem = Problem::of(task, router_client);
    Solution current_solution(problem);
    Solution best_solution(current_solution);
    Cost best_cost = estimate(best_solution);

    for (int k = 0; k < current_solution.getWaypoints().size(); ++k) {
        for (int i = 0; i < current_solution.getWaypoints().size(); ++i) {
            for (int j = i + 2; j <= current_solution.getWaypoints().size(); ++j) {
                std::reverse(current_solution.getWaypoints().begin() + i, current_solution.getWaypoints().begin() + j);
                Cost current_cost = estimate(current_solution);
                if (current_cost < best_cost) {
                    best_solution = current_solution;
                    best_cost = current_cost;
                } else {
                    std::reverse(current_solution.getWaypoints().begin() + i, current_solution.getWaypoints().begin() + j);
                }
            }
        }
    }

    return best_solution.toTspSolution();
}

TwoOptTspAlgorithm::Cost TwoOptTspAlgorithm::estimate(const Solution &solution) const
{
    const DistanceMatrix &distance_matrix = solution.getProblem().getDistanceMatrix();

    Scheduler::SchedulerTask scheduler_task;
    std::vector<Scheduler::LocationId> location_ids;
    location_ids.reserve(solution.getWaypoints().size());
    for (const Waypoint &wp : solution.getWaypoints()) {
        scheduler_task.add_jobs()->CopyFrom(wp.getJob().getData());
        location_ids.push_back(wp.getJob().getLocation().getId());
    }
    scheduler_task.mutable_resource()->CopyFrom(solution.getProblem().getResource().getData());

    Scheduler::SchedulerResult scheduler_result = scheduler.buildSchedule(scheduler_task, solution.getProblem().getDistanceMatrix(), location_ids,
                                                                          solution.getProblem().getResource().getRoutingOptions().getId());

    if (!scheduler_result.has_schedule()) {
        const char *message = "TSP solver wasn't able to build schedule to estimate";
        SPDLOG_ERROR(message);
        throw std::runtime_error(message);
    }

    return estimator.estimate(solution.getProblem().getResource().getData(), scheduler_result.schedule());
}
