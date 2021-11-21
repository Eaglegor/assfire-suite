#pragma once

#include "WaybillSchedulingContext.hpp"
#include "EngineWaybillSchedulerFactory.hpp"
#include "assfire/scheduler/api/WaybillSchedulerApi.hpp"
#include "assfire/scheduler/api/WaybillScheduler.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"

namespace assfire::scheduler {
    class SchedulerEngine : public WaybillSchedulerApi {
    public:
        SchedulerEngine(const WaybillSchedulingContext& context);

        WaybillScheduler createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const override;
        Waybill scheduleWaybill(const Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const override;
        void scheduleWaybillInPlace(Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const override;

    private:
        EngineWaybillSchedulerFactory scheduler_factory;
    };
}