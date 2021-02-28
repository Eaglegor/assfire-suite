#pragma once
#include "Waybill.hpp"
#include "WaybillSchedulingAlgorithm.hpp"
#include <memory>

namespace assfire::scheduler
{
    class WaybillScheduler {
    public:
        WaybillScheduler(std::unique_ptr<WaybillSchedulingAlgorithm> algorithm);

        Waybill scheduleWaybill(const Waybill& waybill_prototype) const;
        void scheduleWaybillInPlace(Waybill& waybill) const;

    private:
        std::unique_ptr<WaybillSchedulingAlgorithm> algorithm;
    };
}