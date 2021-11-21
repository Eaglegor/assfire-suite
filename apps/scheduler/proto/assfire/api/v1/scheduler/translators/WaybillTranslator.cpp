#include "WaybillTranslator.hpp"
#include "WaybillAllocationTranslator.hpp"
#include "assfire/api/v1/scheduler/translators/TimeWindowTranslator.hpp"

using namespace assfire::api::v1::scheduler;

WaybillTranslator::ApiWaybill WaybillTranslator::fromProto(const WaybillTranslator::ProtoWaybill &obj) {
    ApiWaybill::Allocations allocations;
    for (const auto &a : obj.allocations()) {
        allocations.push_back(WaybillAllocationTranslator::fromProto(a));
    }
    return ApiWaybill(TimeWindowTranslator::fromProto(obj.work_shift()), allocations);
}

WaybillTranslator::ProtoWaybill WaybillTranslator::toProto(const WaybillTranslator::ApiWaybill &obj) {
    ProtoWaybill result;
    result.mutable_work_shift()->CopyFrom(TimeWindowTranslator::toProto(obj.getWorkShift()));
    for (const auto &a : obj.getAllocations()) {
        result.add_allocations()->CopyFrom(WaybillAllocationTranslator::toProto(a));
    }
    return result;
}
