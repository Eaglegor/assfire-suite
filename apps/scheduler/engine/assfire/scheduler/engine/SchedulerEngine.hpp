#pragma once

#include "WaybillSchedulerFactory.hpp"
#include "assfire/scheduler/api/WaybillScheduler.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"

namespace assfire::scheduler {
    class SchedulerEngine {
    public:
        SchedulerEngine(WaybillSchedulingContext context);

        WaybillScheduler createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const;
        Waybill scheduleWaybill(const Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const;
        void scheduleWaybillInPlace(Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const;

    private:
        WaybillSchedulingContext context;
        WaybillSchedulerFactory scheduler_factory;
    };
}