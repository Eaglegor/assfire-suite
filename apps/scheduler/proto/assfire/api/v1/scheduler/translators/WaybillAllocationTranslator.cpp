#include "WaybillAllocationTranslator.hpp"
#include "TimeWindowTranslator.hpp"
#include "assfire/api/v1/concepts/translators/TimePointTranslator.hpp"
#include "assfire/api/v1/concepts/translators/TimeIntervalTranslator.hpp"
#include "assfire/api/v1/router/translators/RouteInfoTranslator.hpp"
#include "assfire/api/v1/concepts/translators/LocationTranslator.hpp"

using namespace assfire::api::v1::scheduler;

using TimePointTranslator = assfire::api::v1::concepts::TimePointTranslator;
using TimeIntervalTranslator = assfire::api::v1::concepts::TimeIntervalTranslator;
using LocationTranslator = assfire::api::v1::concepts::LocationTranslator;
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
            RouteInfoTranslator::fromProto(obj.next_route()),
            LocationTranslator::fromProto(obj.location())
            );
}

WaybillAllocationTranslator::ProtoWaybillAllocation WaybillAllocationTranslator::toProto(const WaybillAllocationTranslator::ApiWaybillAllocation &obj) {
    ProtoWaybillAllocation result;

    result.mutable_start_time()->CopyFrom(TimePointTranslator::toProto(obj.getStartTime()));
    result.mutable_end_time()->CopyFrom(TimePointTranslator::toProto(obj.getEndTime()));
    result.mutable_next_route()->CopyFrom(RouteInfoTranslator::toProto(obj.getNextRouteInfo()));
    result.mutable_planned_duration()->CopyFrom(TimeIntervalTranslator::toProto(obj.getPlannedDuration()));
    result.mutable_location()->CopyFrom(LocationTranslator::toProto(obj.getLocation()));

    for(const auto& tw : obj.getTimeWindows()) {
        result.add_time_windows()->CopyFrom(TimeWindowTranslator::toProto(tw));
    }

    return result;
}
