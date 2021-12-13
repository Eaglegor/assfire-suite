#pragma once

#include <assfire/router/api/RouteInfo.hpp>
#include "assfire/locations/api/Location.hpp"
#include <assfire/router/api/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/api/RouterEngineType.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "transport/ProtobufClient.hpp"

namespace assfire::router {
    class GrpcRouteProviderEngine : public RouteProviderEngine {
    public:
        GrpcRouteProviderEngine(const ProtobufClient &client, RouterEngineType engineType, const RouteProviderSettings &settings, const RoutingProfile &routingProfile);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

        Matrix<RouteInfo> getRouteInfoMatrix(const LocationsList &origins, const LocationsList &destinations) const override;
        Matrix<RouteDetails> getRouteDetailsMatrix(const LocationsList &origins, const LocationsList &destinations) const override;

    private:
        const ProtobufClient& client;
        RouterEngineType engine_type;
        RouteProviderSettings settings;
        RoutingProfile routing_profile;
    };
}