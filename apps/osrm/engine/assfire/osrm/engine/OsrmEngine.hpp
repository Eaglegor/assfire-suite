#pragma once

#include "assfire/osrm/api/OsrmApi.hpp"
#include "assfire/osrm/api/OsrmSettings.hpp"

namespace assfire::osrm {
    class OsrmEngine : public OsrmApi {
    public:
        OsrmEngine(const OsrmSettings &settings);

        router::RouteInfo getSingleRouteInfo(const locations::Location& from, const locations::Location& to) override;
        router::RouteDetails getSingleRouteDetails(const locations::Location& from, const locations::Location& to) override;
        router::TripInfo getSingleTripInfo(const std::vector<Location> &locations) override;
        router::TripDetails getSingleTripDetails(const std::vector<Location> &locations) override;
        router::Matrix<RouteInfo> getRouteInfoMatrix(const LocationsList &origins, const LocationsList &destinations) override;

    private:
        OsrmSettings settings;
    };
}