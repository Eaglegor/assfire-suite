#pragma once

#include <assfire/api/v1/scheduler/concepts.pb.h>
#include <assfire/scheduler/api/TimeWindow.hpp>

namespace assfire::api::v1::scheduler {
    class TimeWindowTranslator {
    public:
        using ApiTimeWindow = assfire::scheduler::TimeWindow;
        using ProtoTimeWindow = assfire::api::v1::scheduler::TimeWindow;

        static ApiTimeWindow fromProto(const ProtoTimeWindow& obj);
        static ProtoTimeWindow toProto(const ApiTimeWindow& obj);
    };
}
