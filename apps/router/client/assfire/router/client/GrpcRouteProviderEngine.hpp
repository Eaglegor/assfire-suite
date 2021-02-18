#pragma once

#include <assfire/api/router/model/RouteInfo.hpp>
#include <assfire/api/router/model/Location.hpp>
#include <assfire/api/router/model/RouteDetails.hpp>
#include "assfire/api/router/RouteProviderEngine.hpp"
#include "assfire/api/router/model/RoutingProfile.hpp"
#include "assfire/api/router/RouterEngineType.hpp"
#include "assfire/api/router/RouteProviderSettings.hpp"
#include "assfire/api/router/model/RoutingProfile.hpp"
#include "transport/GrpcClient.hpp"
#include <random>

namespace assfire::router {
    class GrpcRouteProviderEngine : public RouteProviderEngine {
    public:
        GrpcRouteProviderEngine(const GrpcClient &client, RouterEngineType engineType, const RouteProviderSettings &settings, const RoutingProfile &routingProfile);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        const GrpcClient& client;
        RouterEngineType engine_type;
        RouteProviderSettings settings;
        RoutingProfile routing_profile;
    };
}