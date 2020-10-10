#pragma once

#include <vector>
#include "assfire/scheduler/transport/SchedulingAlgorithm.hpp"

namespace assfire::scheduler::transport
{
    class BaseSchedulingAlgorithm : public SchedulingAlgorithm
    {
    public:
        using RouterClient = router::RouterClient;
        using DistanceMatrix = router::DistanceMatrix;
        using LocationId = DistanceMatrix::LocationId;
        using RoutingOptionsId = DistanceMatrix::RoutingOptionsId;
        using RouteInfo = DistanceMatrix::RouteInfo;

        using Resource = assfire::api::v1::model::optimization::transport::Resource;
        using Allocation = assfire::api::v1::service::scheduler::transport::Allocation;
        using Job = assfire::api::v1::model::optimization::transport::Job;
        using RouteInfo = assfire::router::DistanceMatrix::RouteInfo;
        using TimeWindow = assfire::api::v1::model::optimization::transport::TimeWindow;
        using TimePoint = decltype(TimeWindow().start_time());

        BaseSchedulingAlgorithm(const router::RouterClient &router_client);

        virtual ~BaseSchedulingAlgorithm() = default;

        SchedulerResult buildSchedule(const SchedulerTask &task) final override;

        SchedulerResult buildSchedule(const SchedulerTask &task, const router::DistanceMatrix &distance_matrix) final override;

        SchedulerResult buildSchedule(const SchedulerTask &task, const DistanceMatrix &distance_matrix, const std::vector<LocationId> &location_ids, RoutingOptionsId routing_options_id) final override;

    private:
        virtual void schedule(const SchedulerTask &task, SchedulerResult& result) = 0;

        const RouterClient &router_client;
    };
}