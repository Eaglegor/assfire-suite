#pragma once

#include "SchedulerGrpcConnector.hpp"
#include "ClientWaybillSchedulerFactory.hpp"
#include "assfire/scheduler/api/WaybillSchedulerApi.hpp"
#include "assfire/scheduler/api/WaybillScheduler.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"

namespace assfire::scheduler
{
    class SchedulerClient : public WaybillSchedulerApi
    {
    public:
        SchedulerClient(const std::string &host, int port, bool use_ssl);
        SchedulerClient(std::unique_ptr<SchedulerGrpcConnector> grpc_connector);

        WaybillScheduler createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const override;
        Waybill scheduleWaybill(const Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const override;
        void scheduleWaybillInPlace(Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const override;

    private:
        ClientWaybillSchedulerFactory scheduler_factory;
    };
}