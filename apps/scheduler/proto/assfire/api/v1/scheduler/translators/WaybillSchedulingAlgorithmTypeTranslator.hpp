#pragma once

#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "assfire/api/v1/scheduler/concepts.pb.h"

namespace assfire::api::v1::scheduler {
    class WaybillSchedulingAlgorithmTypeTranslator {
    public:
        using ProtoWaybillSchedulingAlgorithmType = assfire::api::v1::scheduler::WaybillSchedulingAlgorithmType;
        using ApiWaybillSchedulingAlgorithmType = assfire::scheduler::WaybillSchedulingAlgorithmType;

        static ApiWaybillSchedulingAlgorithmType fromProto(ProtoWaybillSchedulingAlgorithmType type);
        static ProtoWaybillSchedulingAlgorithmType toProto(ApiWaybillSchedulingAlgorithmType type);
    };
}