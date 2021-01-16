#pragma once

#include "TimePoint.hpp"

namespace assfire::router
{
    using TimeInterval = decltype(TimePoint() - TimePoint());
}