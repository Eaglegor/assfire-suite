#pragma once

#include "CostFunction.hpp"
#include "cost_functions/TotalTimeCostFunction.hpp"
#include "cost_functions/TotalDistanceCostFunction.hpp"
#include "cost_functions/ResourceUsageCostFunction.hpp"

namespace assfire::estimator::transport
{
    class ScheduleEstimator
    {
    public:
        using Schedule = CostFunction::Schedule;
        using Resource = CostFunction::Resource;
        using Allocation = CostFunction::Allocation;
        using CostFunctionType = assfire::api::v1::model::optimization::transport::CostFunctionType;
        using Cost = double;

        Cost estimate(const Resource &resource, const Schedule &schedule) const;
        const CostFunction& getCostFunction(const Resource &resource) const;

    private:
        TotalDistanceCostFunction total_distance_cost_function;
        TotalTimeCostFunction total_time_cost_function;
        ResourceUsageCostFunction resource_usage_cost_function;
    };
}

