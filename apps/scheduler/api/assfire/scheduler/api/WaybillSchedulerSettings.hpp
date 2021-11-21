#pragma once

#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "assfire/router/api/RouterEngineType.hpp"

namespace assfire::scheduler {
    class WaybillSchedulerSettings {
    public:
        WaybillSchedulerSettings(const router::RouteProviderSettings &route_provider_settings, router::RouterEngineType router_engine_type) :
                route_provider_settings(route_provider_settings), router_engine_type(router_engine_type) {}

        WaybillSchedulerSettings(const WaybillSchedulerSettings &rhs) = default;

        const router::RouteProviderSettings &getRouteProviderSettings() const {
            return route_provider_settings;
        }

        router::RouterEngineType getRouterEngineType() const {
            return router_engine_type;
        }

    private:
        router::RouterEngineType router_engine_type;
        router::RouteProviderSettings route_provider_settings;
    };
}