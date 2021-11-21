#pragma once

namespace assfire::scheduler {
    class Waybill;

    class WaybillSchedulingAlgorithm {
    public:
        virtual ~WaybillSchedulingAlgorithm() = default;

        virtual void scheduleWaybill(Waybill& waybill) const = 0;
    };
}