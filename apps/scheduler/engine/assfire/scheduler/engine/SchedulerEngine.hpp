#pragma once

#include "WaybillSchedulerFactory.hpp"
#include "assfire/scheduler/api/WaybillScheduler.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"

namespace assfire::scheduler {
    class SchedulerEngine {
    public:
        WaybillScheduler createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const WaybillSchedulingContext& context) const;
        Waybill scheduleWaybill(const Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const WaybillSchedulingContext& context) const;
        void scheduleWaybillInPlace(Waybill& waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const WaybillSchedulingContext& context) const;

    private:
        WaybillSchedulerFactory scheduler_factory;
    };
}