#pragma once

#include "TimeWindowTranslator.hpp"
#include "WaybillAllocationTranslator.hpp"
#include "WaybillTranslator.hpp"

namespace assfire {
    using TimeWindowTranslator = assfire::api::v1::scheduler::TimeWindowTranslator;
    using WaybillAllocationTranslator = assfire::api::v1::scheduler::WaybillAllocationTranslator;
    using WaybillTranslator = assfire::api::v1::scheduler::WaybillTranslator;
}