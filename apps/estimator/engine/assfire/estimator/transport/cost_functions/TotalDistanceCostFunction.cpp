#include "TotalDistanceCostFunction.hpp"
#include <numeric>

using namespace assfire::estimator::transport;

CostFunction::Cost TotalDistanceCostFunction::estimate(const Resource& resource, const Schedule &schedule) const
{
    return std::accumulate(schedule.allocations().begin(), schedule.allocations().end(), Cost(0),
                           [&](Cost current_cost, const Allocation &alloc) {
                               return current_cost + alloc.route_to_next().distance();
                           });
}
