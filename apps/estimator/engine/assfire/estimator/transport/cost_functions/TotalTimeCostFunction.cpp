#include "TotalTimeCostFunction.hpp"
#include <numeric>

using namespace assfire::estimator::transport;

CostFunction::Cost TotalTimeCostFunction::estimate(const Resource& resource, const Schedule &schedule) const
{
    return std::accumulate(schedule.allocations().begin(), schedule.allocations().end(), Cost(0),
                           [&](Cost current_cost, const Allocation &alloc) {
                               return current_cost
                               + (alloc.allocated_time().end_time() - alloc.allocated_time().start_time())
                               + alloc.route_to_next().duration();
                           });
}
