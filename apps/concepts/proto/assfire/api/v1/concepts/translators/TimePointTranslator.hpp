#pragma once

#include <assfire/api/v1/concepts/concepts.pb.h>
#include "assfire/concepts/TimePoint.hpp"

namespace assfire::api::v1::concepts {
    class TimePointTranslator {
    public:
        using ProtoTimePoint = assfire::api::v1::concepts::TimePoint;
        using ApiTimePoint = assfire::TimePoint;

        static ApiTimePoint fromProto(const ProtoTimePoint &location);

        static ProtoTimePoint toProto(const ApiTimePoint &location);
    };
}