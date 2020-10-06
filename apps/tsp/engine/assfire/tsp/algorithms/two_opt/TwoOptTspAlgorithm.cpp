#include "TwoOptTspAlgorithm.hpp"
#include "assfire/router/client/RouterClient.hpp"
#include "assfire/tsp/algorithms/model/Problem.hpp"
#include <vector>

using namespace assfire::tsp;
using namespace assfire::router;

TwoOptTspAlgorithm::TwoOptTspAlgorithm(const RouterClient &routing_client)
        : router_client(routing_client)
{

}

TspAlgorithm::Solution
TwoOptTspAlgorithm::solveTsp(const TspAlgorithm::Task &task, const TspAlgorithm::Settings &settings,
                             const TspAlgorithm::StatusConsumer &consumer,
                             const InterruptCondition &interruptCondition)
{
    Problem problem = Problem::of(task, router_client);

    return assfire::tsp::TspAlgorithm::Solution();
}

std::int64_t TwoOptTspAlgorithm::estimate(const std::vector<Job> &, const Resource &)
{
    return 0;
}
