#pragma once

#include "TimeWindowTranslator.hpp"
#include "WaybillAllocationTranslator.hpp"
#include "WaybillTranslator.hpp"
#include "WaybillSchedulingAlgorithmTypeTranslator.hpp"
#include "WaybillSchedulerSettingsTranslator.hpp"

namespace assfire {
    using TimeWindowTranslator = assfire::api::v1::scheduler::TimeWindowTranslator;
    using WaybillAllocationTranslator = assfire::api::v1::scheduler::WaybillAllocationTranslator;
    using WaybillSchedulingAlgorithmTypeTranslator = assfire::api::v1::scheduler::WaybillSchedulingAlgorithmTypeTranslator;
    using WaybillSchedulerSettingsTranslator = assfire::api::v1::scheduler::WaybillSchedulerSettingsTranslator;
    using WaybillTranslator = assfire::api::v1::scheduler::WaybillTranslator;
}