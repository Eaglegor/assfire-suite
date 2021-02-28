#pragma once

#include "assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp"

namespace assfire::scheduler {
    class LinearWaybillSchedulingAlgorithm : public WaybillSchedulingAlgorithm {
    public:
        void scheduleWaybill(Waybill &waybill) const override;
    };
}


