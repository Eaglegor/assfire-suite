#pragma once

#include "RouteInfoTranslator.hpp"
#include "RouteProviderSettingsTranslator.hpp"
#include "RouterEngineTypeTranslator.hpp"
#include "RoutingProfileTranslator.hpp"

namespace assfire {
    using RouteInfoTranslator = assfire::api::v1::router::RouteInfoTranslator;
    using RouteProviderSettingsTranslator = assfire::api::v1::router::RouteProviderSettingsTranslator;
    using RouterEngineTypeTranslator = assfire::api::v1::router::RouterEngineTypeTranslator;
    using RoutingProfileTranslator = assfire::api::v1::router::RoutingProfileTranslator;
}