#include "TimePointTranslator.hpp"

using namespace assfire::api::v1::concepts;

TimePointTranslator::ApiTimePoint TimePointTranslator::fromProto(const ProtoTimePoint &obj)
{
    return ApiTimePoint::fromEpochSeconds(obj.epoch_seconds());
}

TimePointTranslator::ProtoTimePoint TimePointTranslator::toProto(const ApiTimePoint &obj)
{
    ProtoTimePoint result;
    result.set_epoch_seconds(obj.toEpochSeconds());
    return result;
}
