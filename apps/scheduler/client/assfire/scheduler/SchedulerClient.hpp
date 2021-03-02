#pragma once

#include "SchedulerGrpcConnector.hpp"
#include "ClientWaybillSchedulerFactory.hpp"
#include "assfire/scheduler/api/WaybillScheduler.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"

namespace assfire::scheduler
{
    class SchedulerClient
    {
    public:
        SchedulerClient(const std::string &host, int port, bool use_ssl);
        SchedulerClient(std::unique_ptr<SchedulerGrpcConnector> grpc_connector);

        WaybillScheduler createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const;
        Waybill scheduleWaybill(const Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const;
        void scheduleWaybillInPlace(Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const;

    private:
        ClientWaybillSchedulerFactory scheduler_factory;
        std::unique_ptr<SchedulerGrpcConnector> grpc_connector;
    };
}