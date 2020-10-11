#include "ScheduleEstimator.hpp"
#include "cost_functions/TotalTimeCostFunction.hpp"
#include "cost_functions/TotalDistanceCostFunction.hpp"
#include "cost_functions/ResourceUsageCostFunction.hpp"
#include <spdlog/spdlog.h>

using namespace assfire::estimator::transport;

ScheduleEstimator::Cost ScheduleEstimator::estimate(const Resource &resource, const Schedule &schedule) const
{
    return getCostFunction(resource).estimate(resource, schedule);
}

const CostFunction &ScheduleEstimator::getCostFunction(const ScheduleEstimator::Resource &resource) const
{
    if (!resource.has_cost_function()) {
        SPDLOG_WARN("Cost function wasn't provided for resource {}. Assuming default cost model", resource.id());
        return resource_usage_cost_function;
    }
    switch (resource.cost_function().type()) {
        case api::v1::model::optimization::transport::COST_FUNCTION_TYPE_TOTAL_DISTANCE: {
            return total_distance_cost_function;
        }
        case api::v1::model::optimization::transport::COST_FUNCTION_TYPE_TOTAL_TIME : {
            return total_time_cost_function;
        }
        case api::v1::model::optimization::transport::COST_FUNCTION_TYPE_UNSPECIFIED:
        case api::v1::model::optimization::transport::COST_FUNCTION_TYPE_RESOURCE_USAGE_COST: {
            return resource_usage_cost_function;
        }
        default: {
            SPDLOG_ERROR("Unknown cost function type: {}", resource.cost_function().type());
            throw std::invalid_argument("Unknown cost function type");
        }
    }
}
