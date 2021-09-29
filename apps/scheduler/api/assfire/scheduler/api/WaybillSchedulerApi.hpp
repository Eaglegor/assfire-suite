#pragma once

#include "assfire/scheduler/api/WaybillScheduler.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"

namespace assfire::scheduler {
    class WaybillSchedulerApi {
    public:
        virtual WaybillScheduler createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const router::RoutingProfile &routing_profile) const = 0;
        virtual Waybill scheduleWaybill(const Waybill &waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const router::RoutingProfile &routing_profile) const = 0;
        virtual void scheduleWaybillInPlace(Waybill &waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const router::RoutingProfile &routing_profile) const = 0;
    };
}