#include "WaybillScheduler.hpp"

using namespace assfire::scheduler;

WaybillScheduler::WaybillScheduler(std::unique_ptr<WaybillSchedulingAlgorithm> algorithm)
        : algorithm(std::move(algorithm)) {}

Waybill WaybillScheduler::scheduleWaybill(const Waybill &waybill_prototype) const {
    Waybill copy(waybill_prototype);
    scheduleWaybillInPlace(copy);
    return copy;
}

void WaybillScheduler::scheduleWaybillInPlace(Waybill &waybill) const {
    algorithm->scheduleWaybill(waybill);
}
