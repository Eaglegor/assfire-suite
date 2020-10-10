#pragma once

#include "assfire/api/v1/service/scheduler/transport/transport.pb.h"
#include "assfire/router/client/RouterClient.hpp"

namespace assfire::scheduler::transport
{
    class SchedulingAlgorithm
    {
    public:
        using SchedulerTask = assfire::api::v1::service::scheduler::transport::SchedulerTask;
        using SchedulerResult = assfire::api::v1::service::scheduler::transport::SchedulerResult;

        virtual ~SchedulingAlgorithm() = default;

        virtual SchedulerResult buildSchedule(const SchedulerTask &task) = 0;

        virtual SchedulerResult buildSchedule(const SchedulerTask &task, const router::DistanceMatrix &distance_matrix) = 0;

        virtual SchedulerResult
        buildSchedule(const SchedulerTask &task, const router::DistanceMatrix &distance_matrix, const std::vector<router::DistanceMatrix::LocationId> &location_ids, router::DistanceMatrix::RoutingOptionsId routing_options_id) = 0;
    };
}