#include "SchedulerEngine.hpp"

using namespace assfire::scheduler;
using namespace assfire::router;

WaybillScheduler SchedulerEngine::createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const WaybillSchedulingContext &context) const {
    return scheduler_factory.createWaybillScheduler(type, settings, context);
}

Waybill SchedulerEngine::scheduleWaybill(const Waybill &waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const WaybillSchedulingContext &context) const {
    return createWaybillScheduler(type, settings, context).scheduleWaybill(waybill);
}

void SchedulerEngine::scheduleWaybillInPlace(Waybill &waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const WaybillSchedulingContext &context) const {
    createWaybillScheduler(type, settings, context).scheduleWaybillInPlace(waybill);
}
