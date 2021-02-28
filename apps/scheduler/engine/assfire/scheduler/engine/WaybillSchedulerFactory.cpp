#include "WaybillSchedulerFactory.hpp"
#include "algorithms/LinearWaybillSchedulingAlgorithm.hpp"
#include "algorithms/BilinearWaybillSchedulingAlgorithm.hpp"
#include <stdexcept>

using namespace assfire::scheduler;

std::unique_ptr<WaybillSchedulingAlgorithm> WaybillSchedulerFactory::createWaybillSchedulingAlgorithm(WaybillSchedulingAlgorithmType algorithm_type) const {
    if(algorithm_type == WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO) {
        algorithm_type = WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR;
    }

    switch(algorithm_type) {
        case WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_LINEAR:
            return std::make_unique<LinearWaybillSchedulingAlgorithm>();
        case WaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR:
            return std::make_unique<BilinearWaybillSchedulingAlgorithm>();
        default:
            throw std::runtime_error("Unknown waybill scheduling algorithm");
    }
}
