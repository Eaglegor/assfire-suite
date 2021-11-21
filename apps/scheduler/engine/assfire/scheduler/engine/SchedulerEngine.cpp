#include "SchedulerEngine.hpp"

using namespace assfire::scheduler;
using namespace assfire::router;

SchedulerEngine::SchedulerEngine(const WaybillSchedulingContext& context) : scheduler_factory(context) {}

WaybillScheduler SchedulerEngine::createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const router::RoutingProfile& routing_profile) const {
    return scheduler_factory.createWaybillScheduler(type, settings, routing_profile);
}

Waybill SchedulerEngine::scheduleWaybill(const Waybill &waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const router::RoutingProfile& routing_profile) const {
    return createWaybillScheduler(type, settings, routing_profile).scheduleWaybill(waybill);
}

void SchedulerEngine::scheduleWaybillInPlace(Waybill &waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const router::RoutingProfile& routing_profile) const {
    createWaybillScheduler(type, settings, routing_profile).scheduleWaybillInPlace(waybill);
}
