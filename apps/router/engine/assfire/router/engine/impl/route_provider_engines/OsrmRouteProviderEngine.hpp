#pragma once

#include <assfire/router/api/RouteInfo.hpp>
#include "assfire/locations/api/Location.hpp"
#include <assfire/router/api/RouteDetails.hpp>
#include "assfire/router/api/RouteProviderEngine.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "OsrmConnector.hpp"
#include <random>
#include <memory>
#include "assfire/router/engine/EngineMetricsCollector.hpp"

namespace web::http::client {
    class http_client;
}

namespace assfire::router {
    class OsrmRouteProviderEngine : public RouteProviderEngine {
    public:
        OsrmRouteProviderEngine(const RoutingProfile &routingProfile, const OsrmGeometry& geometry, std::unique_ptr<OsrmConnector> connector);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;
        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

    private:
    public:
        TripInfo getTripInfo(const LocationsList &locations) const override;

        TripDetails getTripDetails(const LocationsList &locations) const override;

    private:
        RouteDetails calculateRouteDetails(const Location &origin, const Location &destination) const;

        RoutingProfile routing_profile;
        OsrmGeometry geometry;
        std::unique_ptr<OsrmConnector> client;
        mutable EngineMetricsCollector metrics_collector;
    };
}