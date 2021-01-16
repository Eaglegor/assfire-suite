#pragma once

#include <assfire/engine/router/model/RouteInfo.hpp>
#include <assfire/engine/router/model/Location.hpp>
#include <assfire/engine/router/model/RouteDetails.hpp>
#include "../../RouteProviderEngine.hpp"
#include "../../RoutingProfile.hpp"
#include "../../RouteProviderSettings.hpp"
#include <random>
#include <memory>

namespace web::http::client {
    class http_client;
}

namespace assfire::router {
    class OsrmRouteProviderEngine : public RouteProviderEngine {
    public:
        OsrmRouteProviderEngine(const std::string &osrm_endpoint, const RoutingProfile &routingProfile, const RouteProviderSettings::Osrm::Geometry& geometry);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
        RoutingProfile routing_profile;
        std::string endpoint;
        RouteProviderSettings::Osrm::Geometry geometry;
        std::unique_ptr<web::http::client::http_client> client;
    };
}