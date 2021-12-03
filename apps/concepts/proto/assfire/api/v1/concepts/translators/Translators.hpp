#pragma once

#include "DistanceTranslator.hpp"
#include "SpeedTranslator.hpp"
#include "TimeIntervalTranslator.hpp"
#include "TimePointTranslator.hpp"

namespace assfire {
    using DistanceTranslator = assfire::api::v1::concepts::DistanceTranslator;
    using SpeedTranslator = assfire::api::v1::concepts::SpeedTranslator;
    using TimeIntervalTranslator = assfire::api::v1::concepts::TimeIntervalTranslator;
    using TimePointTranslator = assfire::api::v1::concepts::TimePointTranslator;
}