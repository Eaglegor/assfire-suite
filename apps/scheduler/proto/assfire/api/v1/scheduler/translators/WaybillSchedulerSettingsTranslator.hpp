#pragma once

#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"
#include "assfire/api/v1/scheduler/concepts.pb.h"

namespace assfire::api::v1::scheduler {
    class WaybillSchedulerSettingsTranslator {
    public:
        using ProtoWaybillSchedulerSettings = assfire::api::v1::scheduler::WaybillSchedulerSettings;
        using ApiWaybillSchedulerSettings = assfire::scheduler::WaybillSchedulerSettings;

        static ApiWaybillSchedulerSettings fromProto(const ProtoWaybillSchedulerSettings& obj);
        static ProtoWaybillSchedulerSettings toProto(const ApiWaybillSchedulerSettings& obj);
    };
}


