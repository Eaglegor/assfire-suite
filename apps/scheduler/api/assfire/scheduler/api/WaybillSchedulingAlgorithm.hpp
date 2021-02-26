#pragma once

namespace assfire::scheduler {
    class WaybillSchedulingAlgorithm {
    public:
        virtual ~WaybillSchedulingAlgorithm() = default;

        virtual void scheduleWaybill(Waybill& waybill_prototype) const = 0;
    };
}