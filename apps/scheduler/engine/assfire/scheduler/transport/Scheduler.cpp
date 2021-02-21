#include "Scheduler.hpp"
#include "SchedulingAlgorithm.hpp"
#include "spdlog/spdlog.h"
#include "algorithms/BilinearSchedulingAlgorithm.hpp"
#include "algorithms/LinearSchedulingAlgorithm.hpp"

using namespace assfire::scheduler::transport;

Scheduler::Scheduler(const RouterClient &router_client)
        : router_client(router_client)
{}

Scheduler::SchedulerResult Scheduler::buildSchedule(const SchedulerTask &task) const
{
    switch (task.algorithm()) {
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_LINEAR: {
            return LinearSchedulingAlgorithm(router_client).buildSchedule(task);
        }
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_UNSPECIFIED:
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_BILINEAR: {
            return BilinearSchedulingAlgorithm(router_client).buildSchedule(task);
        }
        default: {
            SPDLOG_ERROR("Unknown scheduler algorithm: {}", task.algorithm());
            throw std::invalid_argument("Unknown scheduler algorithm: " + std::to_string(task.algorithm()));
        }
    }
}

Scheduler::SchedulerResult Scheduler::buildSchedule(const Scheduler::SchedulerTask &task, const DistanceMatrix &distance_matrix) const
{
    switch (task.algorithm()) {
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_LINEAR: {
            return LinearSchedulingAlgorithm(router_client).buildSchedule(task, distance_matrix);
        }
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_UNSPECIFIED:
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_BILINEAR: {
            return BilinearSchedulingAlgorithm(router_client).buildSchedule(task, distance_matrix);
        }
        default: {
            SPDLOG_ERROR("Unknown scheduler algorithm: {}", task.algorithm());
            throw std::invalid_argument("Unknown scheduler algorithm: " + std::to_string(task.algorithm()));
        }
    }
}

Scheduler::SchedulerResult Scheduler::buildSchedule(const SchedulerTask &task, const DistanceMatrix &distance_matrix, const std::vector<LocationId> &location_ids, RoutingOptionsId routing_options_id) const
{
    switch (task.algorithm()) {
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_LINEAR: {
            return LinearSchedulingAlgorithm(router_client).buildSchedule(task, distance_matrix, location_ids, routing_options_id);
        }
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_UNSPECIFIED:
        case api::v1::service::scheduler::transport::SCHEDULING_ALGORITHM_BILINEAR: {
            return BilinearSchedulingAlgorithm(router_client).buildSchedule(task, distance_matrix, location_ids, routing_options_id);
        }
        default: {
            SPDLOG_ERROR("Unknown scheduler algorithm: {}", task.algorithm());
            throw std::invalid_argument("Unknown scheduler algorithm: " + std::to_string(task.algorithm()));
        }
    }
}