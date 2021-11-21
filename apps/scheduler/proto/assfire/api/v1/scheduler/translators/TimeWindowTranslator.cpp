#include "TimeWindowTranslator.hpp"
#include "assfire/api/v1/concepts/translators/TimePointTranslator.hpp"

using namespace assfire::api::v1::scheduler;
using TimePointTranslator = assfire::api::v1::concepts::TimePointTranslator;

TimeWindowTranslator::ApiTimeWindow TimeWindowTranslator::fromProto(const TimeWindowTranslator::ProtoTimeWindow &obj) {
    return ApiTimeWindow(TimePointTranslator::fromProto(obj.start_time()), TimePointTranslator::fromProto(obj.end_time()));
}

TimeWindowTranslator::ProtoTimeWindow TimeWindowTranslator::toProto(const TimeWindowTranslator::ApiTimeWindow &obj) {
    ProtoTimeWindow result;
    result.mutable_start_time()->CopyFrom(TimePointTranslator::toProto(obj.getStartTime()));
    result.mutable_end_time()->CopyFrom(TimePointTranslator::toProto(obj.getEndTime()));
    return result;
}
