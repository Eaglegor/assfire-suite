#include "TimeIntervalTranslator.hpp"

using namespace assfire::api::v1::concepts;

TimeIntervalTranslator::ApiTimeInterval TimeIntervalTranslator::fromProto(const TimeIntervalTranslator::ProtoTimeInterval &obj) {
    return ApiTimeInterval::fromSeconds(obj.seconds());
}

TimeIntervalTranslator::ProtoTimeInterval TimeIntervalTranslator::toProto(const TimeIntervalTranslator::ApiTimeInterval &obj) {
    ProtoTimeInterval result;
    result.set_seconds(obj.toSeconds());
    return result;
}
