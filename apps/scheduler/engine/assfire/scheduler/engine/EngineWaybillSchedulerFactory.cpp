#include "EngineWaybillSchedulerFactory.hpp"
#include "algorithms/LinearWaybillSchedulingAlgorithm.hpp"
#include "algorithms/BilinearWaybillSchedulingAlgorithm.hpp"
#include <stdexcept>

using namespace assfire::scheduler;

assfire::scheduler::EngineWaybillSchedulerFactory::EngineWaybillSchedulerFactory(const assfire::scheduler::WaybillSchedulingContext &scheduling_context) : scheduling_context(scheduling_context) {}

WaybillScheduler EngineWaybillSchedulerFactory::createWaybillScheduler(AlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile) const {
    if(type == WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO) {
        type = WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR;
    }

    switch(type) {
        case WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_LINEAR:
            return WaybillScheduler(std::make_shared<LinearWaybillSchedulingAlgorithm>(scheduling_context.getDistanceMatrixProvider()(settings.getRouterEngineType(), settings.getRouteProviderSettings(), routing_profile)));
        case WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR:
            return WaybillScheduler(std::make_shared<BilinearWaybillSchedulingAlgorithm>(scheduling_context.getDistanceMatrixProvider()(settings.getRouterEngineType(), settings.getRouteProviderSettings(), routing_profile)));
        default:
            throw std::invalid_argument("Unknown waybill scheduling algorithm type");
    }
}
