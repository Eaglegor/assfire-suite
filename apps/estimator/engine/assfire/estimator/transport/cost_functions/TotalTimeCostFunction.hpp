#pragma once

#include "assfire/estimator/transport/CostFunction.hpp"

namespace assfire::estimator::transport
{
    class TotalTimeCostFunction : public CostFunction
    {
    public:
        Cost estimate(const Resource& resource, const Schedule &schedule) const override;
    };
}
