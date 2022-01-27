#include "OsrmEngine.hpp"

namespace assfire::osrm {

    OsrmEngine::OsrmEngine(const OsrmSettings &settings) : settings(settings) {}

    router::RouteInfo OsrmEngine::getSingleRouteInfo(const locations::Location &from, const locations::Location &to) {
        return router::RouteInfo();
    }

    router::RouteDetails OsrmEngine::getSingleRouteDetails(const locations::Location &from, const locations::Location &to) {
        return router::RouteDetails();
    }

    router::TripInfo OsrmEngine::getSingleTripInfo(const std::vector<Location> &locations) {
        return router::TripInfo();
    }

    router::TripDetails OsrmEngine::getSingleTripDetails(const std::vector<Location> &locations) {
        return router::TripDetails();
    }

    router::Matrix<RouteInfo> OsrmEngine::getRouteInfoMatrix(const OsrmApi::LocationsList &origins, const OsrmApi::LocationsList &destinations) {
        return router::Matrix<RouteInfo>(0, 0);
    }
}