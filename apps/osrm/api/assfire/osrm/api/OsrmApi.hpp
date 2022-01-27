#pragma once

#include "assfire/router/api/RouteInfo.hpp"
#include "assfire/router/api/RouteDetails.hpp"
#include "assfire/router/api/TripInfo.hpp"
#include "assfire/router/api/TripDetails.hpp"
#include "assfire/router/api/RouteProviderSettings.hpp"
#include "assfire/router/api/Matrix.hpp"
#include "assfire/locations/api/Location.hpp"

namespace assfire::osrm {
    class OsrmApi {
    public:
        using Location = locations::Location;
        using RouteInfo = router::RouteInfo;
        using RouteDetails = router::RouteDetails;
        using TripInfo = router::TripInfo;
        using TripDetails = router::TripDetails;
        using LocationsList = std::vector<Location>;

        virtual ~OsrmApi() = default;
        virtual router::RouteInfo getSingleRouteInfo(const locations::Location& from, const locations::Location& to) = 0;
        virtual router::RouteDetails getSingleRouteDetails(const locations::Location& from, const locations::Location& to) = 0;
        virtual router::TripInfo getSingleTripInfo(const std::vector<locations::Location> &locations) = 0;
        virtual router::TripDetails getSingleTripDetails(const std::vector<locations::Location> &locations) = 0;
        virtual router::Matrix<RouteInfo> getRouteInfoMatrix(const LocationsList &origins, const LocationsList &destinations) = 0;
    };
}