#include <stdexcept>
#include "WaybillSchedulingAlgorithmTypeTranslator.hpp"

using namespace assfire::api::v1::scheduler;

WaybillSchedulingAlgorithmTypeTranslator::ApiWaybillSchedulingAlgorithmType
WaybillSchedulingAlgorithmTypeTranslator::fromProto(WaybillSchedulingAlgorithmTypeTranslator::ProtoWaybillSchedulingAlgorithmType type) {
    switch(type) {
        case WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO:
            return ApiWaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO;
        case WAYBILL_SCHEDULING_ALGORITHM_TYPE_LINEAR:
            return ApiWaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_LINEAR;
        case WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR:
            return ApiWaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR;
        default:
            throw std::invalid_argument("Unknown waybill scheduling algorithm type");
    }
}

WaybillSchedulingAlgorithmTypeTranslator::ProtoWaybillSchedulingAlgorithmType
WaybillSchedulingAlgorithmTypeTranslator::toProto(WaybillSchedulingAlgorithmTypeTranslator::ApiWaybillSchedulingAlgorithmType type) {
    switch(type) {
        case ApiWaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO:
            return WAYBILL_SCHEDULING_ALGORITHM_TYPE_AUTO;
        case ApiWaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_LINEAR:
            return WAYBILL_SCHEDULING_ALGORITHM_TYPE_LINEAR;
        case ApiWaybillSchedulingAlgorithmType::WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR:
            return WAYBILL_SCHEDULING_ALGORITHM_TYPE_BILINEAR;
        default:
            throw std::invalid_argument("Unknown waybill scheduling algorithm type");
    }
}
