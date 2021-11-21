#pragma once

#include <memory>
#include "assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp"
#include "assfire/scheduler/api/WaybillScheduler.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "WaybillSchedulingContext.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"

namespace assfire::scheduler {
    class EngineWaybillSchedulerFactory {
    public:
        using AlgorithmType = WaybillSchedulingAlgorithmType;

        EngineWaybillSchedulerFactory(const WaybillSchedulingContext &scheduling_context);

        WaybillScheduler createWaybillScheduler(AlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const ;

    private:
        WaybillSchedulingContext scheduling_context;
    };
}