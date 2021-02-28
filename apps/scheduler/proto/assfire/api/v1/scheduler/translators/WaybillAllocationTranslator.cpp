#include "WaybillAllocationTranslator.hpp"
#include "TimeWindowTranslator.hpp"
#include "assfire/api/v1/concepts/translators/TimePointTranslator.hpp"
#include "assfire/api/v1/concepts/translators/TimeIntervalTranslator.hpp"
#include "assfire/api/v1/router/translators/RouteInfoTranslator.hpp"

using namespace assfire::api::v1::scheduler;

using TimePointTranslator = assfire::api::v1::concepts::TimePointTranslator;
using TimeIntervalTranslator = assfire::api::v1::concepts::TimeIntervalTranslator;
using TimeWindowTranslator = assfire::api::v1::scheduler::TimeWindowTranslator;
using RouteInfoTranslator = assfire::api::v1::router::RouteInfoTranslator;

WaybillAllocationTranslator::ApiWaybillAllocation WaybillAllocationTranslator::fromProto(const WaybillAllocationTranslator::ProtoWaybillAllocation &obj) {
    ApiWaybillAllocation::TimeWindows time_windows;
    for(const auto& tw : obj.time_windows()) {
        time_windows.push_back(TimeWindowTranslator::fromProto(tw));
    }
    return ApiWaybillAllocation(
            TimePointTranslator::fromProto(obj.start_time()),
            TimePointTranslator::fromProto(obj.end_time()),
            TimeIntervalTranslator::fromProto(obj.planned_duration()),
            std::move(time_windows),
            RouteInfoTranslator::fromProto(obj.next_route())
            );
}

WaybillAllocationTranslator::ProtoWaybillAllocation WaybillAllocationTranslator::toProto(const WaybillAllocationTranslator::ApiWaybillAllocation &obj) {
    ProtoWaybillAllocation result;

    result.mutable_start_time()->CopyFrom(TimePointTranslator::toProto(obj.getStartTime()));
    result.mutable_end_time()->CopyFrom(TimePointTranslator::toProto(obj.getEndTime()));
    result.mutable_next_route()->CopyFrom(RouteInfoTranslator::toProto(obj.getNextRouteInfo()));
    result.mutable_planned_duration()->CopyFrom(TimeIntervalTranslator::toProto(obj.getPlannedDuration()));

    for(const auto& tw : obj.getTimeWindows()) {
        result.add_time_windows()->CopyFrom(TimeWindowTranslator::toProto(tw));
    }

    return result;
}
