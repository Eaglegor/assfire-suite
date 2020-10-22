#pragma once

#include <memory>
#include "SchedulingAlgorithm.hpp"
#include "assfire/router/client/RouterClient.hpp"

namespace assfire::scheduler::transport
{
    class Scheduler
    {
    public:
        using SchedulerTask = SchedulingAlgorithm::SchedulerTask;
        using SchedulerResult = SchedulingAlgorithm::SchedulerResult;

        using RouterClient = router::RouterClient;
        using DistanceMatrix = router::DistanceMatrix;
        using LocationId = DistanceMatrix::LocationId;
        using RoutingOptionsId = DistanceMatrix::RoutingOptionsId;
        using RouteInfo = DistanceMatrix::RouteInfo;

        Scheduler(const RouterClient &router_client);

        SchedulerResult buildSchedule(const SchedulerTask &) const;

        SchedulerResult buildSchedule(const SchedulerTask &task, const DistanceMatrix &distance_matrix) const;

        SchedulerResult buildSchedule(const SchedulerTask &task, const DistanceMatrix &distance_matrix, const std::vector<LocationId> &location_ids, RoutingOptionsId routing_options_id) const;

    private:
        const RouterClient &router_client;
    };
}