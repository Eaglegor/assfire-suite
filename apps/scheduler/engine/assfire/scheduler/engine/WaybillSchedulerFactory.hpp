#pragma once

#include <memory>
#include "assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp"
#include "assfire/scheduler/api/WaybillScheduler.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "WaybillSchedulingContext.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"
#include "WaybillSchedulingContext.hpp"

namespace assfire::scheduler {
    class WaybillSchedulerFactory {
    public:
        using AlgorithmType = WaybillSchedulingAlgorithmType;
        using AlgorithmPtr = std::unique_ptr<WaybillSchedulingAlgorithm>;

        WaybillScheduler createWaybillScheduler(AlgorithmType type, const WaybillSchedulerSettings& settings, const WaybillSchedulingContext& context) const;
    };
}