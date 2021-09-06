#pragma once
#include "Waybill.hpp"
#include "WaybillSchedulingAlgorithm.hpp"
#include <memory>

namespace assfire::scheduler
{
    class WaybillScheduler {
    public:
        using AlgorithmPtr = std::shared_ptr<WaybillSchedulingAlgorithm>;

        WaybillScheduler(AlgorithmPtr algorithm);

        Waybill scheduleWaybill(const Waybill& waybill_prototype) const;
        void scheduleWaybillInPlace(Waybill& waybill) const;

    private:
        AlgorithmPtr algorithm;
    };
}