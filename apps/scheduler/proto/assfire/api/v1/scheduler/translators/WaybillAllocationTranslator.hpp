#pragma once

#include <assfire/api/v1/scheduler/concepts.pb.h>
#include <assfire/scheduler/api/WaybillAllocation.hpp>

namespace assfire::api::v1::scheduler {
    class WaybillAllocationTranslator {
    public:
        using ApiWaybillAllocation = assfire::scheduler::WaybillAllocation;
        using ProtoWaybillAllocation = assfire::api::v1::scheduler::WaybillAllocation;

        static ApiWaybillAllocation fromProto(const ProtoWaybillAllocation& obj);
        static ProtoWaybillAllocation toProto(const ApiWaybillAllocation& obj);
    };
}


