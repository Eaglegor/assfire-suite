#pragma once

#include <memory>
#include "assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"

namespace assfire::scheduler {
    class WaybillSchedulerFactory {
    public:
        std::unique_ptr<WaybillSchedulingAlgorithm> createWaybillSchedulingAlgorithm(WaybillSchedulingAlgorithmType algorithm_type = WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO) const;
    };
}