#pragma once

#include <assfire/scheduler/api/WaybillScheduler.hpp>
#include <assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp>
#include <assfire/scheduler/api/WaybillSchedulerSettings.hpp>
#include <assfire/router/api/RoutingProfile.hpp>
#include "SchedulerGrpcConnector.hpp"

namespace assfire::scheduler {
    class ClientWaybillSchedulerFactory {
    public:
        WaybillScheduler createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile, const SchedulerGrpcConnector& grpc_connector) const;
    };
}