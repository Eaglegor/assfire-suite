#include "WaybillSchedulerFactory.hpp"
#include "algorithms/LinearWaybillSchedulingAlgorithmTest.hpp"
#include "algorithms/BilinearWaybillSchedulingAlgorithm.hpp"
#include <stdexcept>

using namespace assfire::scheduler;

WaybillScheduler WaybillSchedulerFactory::createWaybillScheduler(AlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile, const WaybillSchedulingContext& context) const {
    if(type == WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO) {
        type = WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR;
    }

    switch(type) {
        case WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_LINEAR:
            return WaybillScheduler(std::make_unique<LinearWaybillSchedulingAlgorithm>(context.getDistanceMatrixProvider()(settings.getRouterEngineType(), settings.getRouteProviderSettings(), routing_profile)));
        case WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR:
            return WaybillScheduler(std::make_unique<BilinearWaybillSchedulingAlgorithm>(context.getDistanceMatrixProvider()(settings.getRouterEngineType(), settings.getRouteProviderSettings(), routing_profile)));
        default:
            throw std::invalid_argument("Unknown waybill scheduling algorithm type");
    }
}
