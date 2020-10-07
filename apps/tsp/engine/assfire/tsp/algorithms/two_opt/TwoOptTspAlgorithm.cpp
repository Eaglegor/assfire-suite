#include "TwoOptTspAlgorithm.hpp"
#include "assfire/router/client/RouterClient.hpp"
#include "assfire/tsp/algorithms/model/Problem.hpp"
#include "assfire/tsp/algorithms/model/Solution.hpp"
#include <vector>

using namespace assfire::tsp;
using namespace assfire::router;

TwoOptTspAlgorithm::TwoOptTspAlgorithm(const RouterClient &routing_client)
        : router_client(routing_client)
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

TwoOptTspAlgorithm::Cost TwoOptTspAlgorithm::estimate(const Solution &solution)
{
    const DistanceMatrix &distance_matrix = solution.getProblem().getDistanceMatrix();

    Cost cost = 0;
    for (int i = 0; i < solution.getWaypoints().size() - 1; ++i) {
        const auto &fromLocation = solution.getWaypoints()[i].getJob().getLocation().getId();
        const auto &toLocation = solution.getWaypoints()[i + 1].getJob().getLocation().getId();
        const auto &routingOptions = solution.getProblem().getResource().getRoutingOptions().getId();

        cost += distance_matrix.getRoute(fromLocation, toLocation, routingOptions).duration();
    }

    return cost;
}
