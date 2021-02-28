#pragma once

#include <assfire/api/v1/concepts/concepts.pb.h>
#include <assfire/concepts/TimeInterval.hpp>

namespace assfire::api::v1::concepts {
    class TimeIntervalTranslator {
    public:
        using ApiTimeInterval = assfire::TimeInterval;
        using ProtoTimeInterval = assfire::api::v1::concepts::TimeInterval;

        static ApiTimeInterval fromProto(const ProtoTimeInterval& obj);
        static ProtoTimeInterval toProto(const ApiTimeInterval& obj);
    };
}