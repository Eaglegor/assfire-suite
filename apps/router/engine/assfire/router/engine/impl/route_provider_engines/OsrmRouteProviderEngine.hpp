#pragma once

#include <assfire/router/api/model/RouteInfo.hpp>
#include <assfire/router/api/model/Location.hpp>
#include <assfire/router/api/model/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/model/RoutingProfile.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "OsrmConnector.hpp"
#include <random>
#include <memory>

namespace web::http::client {
    class http_client;
}

namespace assfire::router {
    class OsrmRouteProviderEngine : public RouteProviderEngine {
    public:
        OsrmRouteProviderEngine(const RoutingProfile &routingProfile, const RouteProviderSettings::Osrm::Geometry& geometry, std::unique_ptr<OsrmConnector> connector);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RoutingProfile routing_profile;
        RouteProviderSettings::Osrm::Geometry geometry;
        std::unique_ptr<OsrmConnector> client;
    };
}