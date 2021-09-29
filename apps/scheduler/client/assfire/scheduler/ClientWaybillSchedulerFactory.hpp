#pragma once

#include <assfire/scheduler/api/WaybillScheduler.hpp>
#include <assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp>
#include <assfire/scheduler/api/WaybillSchedulerSettings.hpp>
#include <assfire/router/api/RoutingProfile.hpp>
#include "SchedulerGrpcConnector.hpp"

namespace assfire::scheduler {
    class ClientWaybillSchedulerFactory {
    public:
        ClientWaybillSchedulerFactory(std::unique_ptr<SchedulerGrpcConnector> grpc_connector);

        WaybillScheduler createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const;

    private:
        std::unique_ptr<SchedulerGrpcConnector> grpc_connector;
    };
}