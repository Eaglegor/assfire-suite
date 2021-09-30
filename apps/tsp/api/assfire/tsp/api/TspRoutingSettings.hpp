#pragma once

#include "assfire/router/api/RouterEngineType.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "assfire/router/api/RoutingProfile.hpp"

namespace assfire::tsp {
    class TspRoutingSettings {
    public:
        TspRoutingSettings(router::RouterEngineType engine_type, const router::RouteProviderSettings &route_provider_settings, const router::RoutingProfile &routing_profile) : engine_type(
                engine_type), route_provider_settings(route_provider_settings), routing_profile(routing_profile) {}

        router::RouterEngineType getEngineType() const {
            return engine_type;
        }

        const router::RouteProviderSettings &getRouteProviderSettings() const {
            return route_provider_settings;
        }

        const router::RoutingProfile &getRoutingProfile() const {
            return routing_profile;
        }

    private:
        router::RouterEngineType engine_type;
        router::RouteProviderSettings route_provider_settings;
        router::RoutingProfile routing_profile;
    };
}