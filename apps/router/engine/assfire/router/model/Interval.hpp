#pragma once

#include "TimePoint.hpp"

namespace assfire::router
{
    using Interval = decltype(TimePoint() - TimePoint());
}