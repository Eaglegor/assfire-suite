#pragma once

#include <assfire/router/api/model/RouteInfo.hpp>
#include "../../../../../../libs/route_optimization_concepts/assfire/concepts/Location.hpp"
#include <assfire/router/api/model/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/model/RoutingProfile.hpp"
#include "assfire/router/api/RouterEngineType.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "assfire/router/api/model/RoutingProfile.hpp"
#include "transport/ProtobufClient.hpp"
#include <random>

namespace assfire::router {
    class GrpcRouteProviderEngine : public RouteProviderEngine {
    public:
        GrpcRouteProviderEngine(const ProtobufClient &client, RouterEngineType engineType, const RouteProviderSettings &settings, const RoutingProfile &routingProfile);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

        Matrix<RouteInfo> getRouteInfoMatrix(const Locations &origins, const Locations &destinations) override;
        Matrix<RouteDetails> getRouteDetailsMatrix(const Locations &origins, const Locations &destinations) override;

    private:
        const ProtobufClient& client;
        RouterEngineType engine_type;
        RouteProviderSettings settings;
        RoutingProfile routing_profile;
    };
}