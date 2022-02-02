#pragma once

#include "assfire/router/api/osrm/OsrmSettings.hpp"
#include "assfire/router/api/RouteProviderEngine.hpp"

namespace assfire::router {
    class OsrmHttpEngine : public RouteProviderEngine {
    public:
        OsrmHttpEngine(const OsrmSettings &settings);

        RouteInfo getSingleRouteInfo(const Location &origin, const Location &destination) const override;

        RouteDetails getSingleRouteDetails(const Location &origin, const Location &destination) const override;

        Matrix<RouteInfo> getRouteInfoMatrix(const LocationsList &origins, const LocationsList &destinations) const override;

        Matrix<RouteDetails> getRouteDetailsMatrix(const LocationsList &origins, const LocationsList &destinations) const override;

        TripInfo getTripInfo(const LocationsList &locations) const override;

        TripDetails getTripDetails(const LocationsList &locations) const override;

    private:
        OsrmSettings settings;
    };
}