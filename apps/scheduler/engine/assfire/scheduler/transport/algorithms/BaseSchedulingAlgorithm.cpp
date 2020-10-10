#include "BaseSchedulingAlgorithm.hpp"
#include <spdlog/spdlog.h>
#include <cstdint>

using namespace assfire::scheduler::transport;

BaseSchedulingAlgorithm::BaseSchedulingAlgorithm(const RouterClient &router_client)
        : router_client(router_client)
{}

SchedulingAlgorithm::SchedulerResult BaseSchedulingAlgorithm::buildSchedule(const SchedulerTask &task)
{
    return buildSchedule(task, router_client.createDistanceMatrix(RouterClient::RequestStrategy::DIRECT_REQUEST));
}

SchedulingAlgorithm::SchedulerResult BaseSchedulingAlgorithm::buildSchedule(const SchedulerTask &task, const DistanceMatrix &distance_matrix)
{
    SchedulerResult result;

    for (int i = 0; i < task.jobs_size(); ++i) {
        Allocation *allocation = result.mutable_schedule()->add_allocations();
        allocation->set_job_id(task.jobs().at(i).id());
        if (i < task.jobs_size() - 1) {
            allocation->mutable_route_to_next()->CopyFrom(distance_matrix.getRoute(task.jobs().at(i).location(), task.jobs().at(i + 1).location(), task.resource().routing_options()));
        } else {
            allocation->mutable_route_to_next()->mutable_origin()->CopyFrom(task.jobs().at(i).location());
            allocation->mutable_route_to_next()->set_distance(0);
            allocation->mutable_route_to_next()->set_duration(0);
        }
    }

    schedule(task, result);

    return result;
}

SchedulingAlgorithm::SchedulerResult BaseSchedulingAlgorithm::buildSchedule(const SchedulerTask &task,
                                                                            const DistanceMatrix &distance_matrix,
                                                                            const std::vector<LocationId> &location_ids,
                                                                            RoutingOptionsId routing_options_id)
{
    if (location_ids.size() != task.jobs_size()) {
        SPDLOG_ERROR("Location ids count ({}) does not match with jobs count ({})", location_ids.size(), task.jobs_size());
        throw std::invalid_argument("Location ids count does not match with jobs count");
    }

    SchedulerResult result;

    for (int i = 0; i < task.jobs_size(); ++i) {
        Allocation *allocation = result.mutable_schedule()->add_allocations();
        allocation->set_job_id(task.jobs().at(i).id());
        if (i < task.jobs_size() - 1) {
            allocation->mutable_route_to_next()->CopyFrom(distance_matrix.getRoute(location_ids.at(i), location_ids.at(i + 1), routing_options_id));
        } else {
            allocation->mutable_route_to_next()->mutable_origin()->CopyFrom(task.jobs().at(i).location());
            allocation->mutable_route_to_next()->set_distance(0);
            allocation->mutable_route_to_next()->set_duration(0);
        }
    }

    schedule(task, result);

    return result;
}
