#pragma once

#include "assfire/api/v1/model/optimization/transport/transport.pb.h"
#include "assfire/api/v1/service/scheduler/transport/transport.pb.h"

namespace assfire::estimator::transport
{
    class CostFunction
    {
    public:
        using Schedule = assfire::api::v1::service::scheduler::transport::Schedule;
        using Resource = assfire::api::v1::model::optimization::transport::Resource;
        using Allocation = assfire::api::v1::service::scheduler::transport::Allocation;
        using Cost = double;

        virtual ~CostFunction() = default;

        virtual Cost estimate(const Resource& resource, const Schedule& schedule) const = 0;
    };
}