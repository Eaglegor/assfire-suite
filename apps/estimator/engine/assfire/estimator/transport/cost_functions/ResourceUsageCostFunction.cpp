#include "ResourceUsageCostFunction.hpp"
#include <numeric>

using namespace assfire::estimator::transport;

CostFunction::Cost ResourceUsageCostFunction::estimate(const CostFunction::Resource &resource, const CostFunction::Schedule &schedule) const
{
    auto resource_usage_costs = resource.cost_function().resource_usage_costs();

    Cost cost = resource_usage_costs.activation_cost();
    for (int i = 0; i < schedule.allocations().size() - 1; ++i) {
        const Allocation &current = schedule.allocations().at(i);
        const Allocation &next = schedule.allocations().at(i + 1);
        cost += (current.allocated_time().end_time() - current.allocated_time().start_time()) * resource_usage_costs.working_time_unit_cost();
        cost += current.route_to_next().duration() * resource_usage_costs.driving_time_unit_cost();
        cost += (next.allocated_time().start_time() - current.allocated_time().end_time() - current.route_to_next().duration()) * resource_usage_costs.waiting_time_unit_cost();
        cost += current.route_to_next().distance() * resource_usage_costs.distance_unit_cost();
    }

    return cost;
}
