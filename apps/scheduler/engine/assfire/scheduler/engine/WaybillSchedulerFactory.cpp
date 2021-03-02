#include "WaybillSchedulerFactory.hpp"
#include "algorithms/LinearWaybillSchedulingAlgorithm.hpp"
#include "algorithms/BilinearWaybillSchedulingAlgorithm.hpp"
#include <stdexcept>

using namespace assfire::scheduler;

WaybillScheduler WaybillSchedulerFactory::createWaybillScheduler(AlgorithmType type, const WaybillSchedulerSettings& settings, const WaybillSchedulingContext& context) const {
    if(type == WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO) {
        type = WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR;
    }

    switch(type) {
        case WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_LINEAR:
            return WaybillScheduler(std::make_unique<LinearWaybillSchedulingAlgorithm>(context.getDistanceMatrix()));
        case WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR:
            return WaybillScheduler(std::make_unique<BilinearWaybillSchedulingAlgorithm>(context.getDistanceMatrix()));
        default:
            throw std::invalid_argument("Unknown waybill scheduling algorithm type");
    }
}
