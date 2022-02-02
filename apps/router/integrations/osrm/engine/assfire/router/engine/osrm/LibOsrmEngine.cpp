#include "LibOsrmEngine.hpp"

namespace assfire::router {
    LibOsrmEngine::LibOsrmEngine(const OsrmSettings &settings) : settings(settings) {}

    RouteInfo LibOsrmEngine::getSingleRouteInfo(const RouteProviderEngine::Location &origin, const RouteProviderEngine::Location &destination) const {
        return RouteInfo();
    }

    RouteDetails LibOsrmEngine::getSingleRouteDetails(const RouteProviderEngine::Location &origin, const RouteProviderEngine::Location &destination) const {
        return RouteDetails();
    }

    Matrix<RouteInfo> LibOsrmEngine::getRouteInfoMatrix(const RouteProviderEngine::LocationsList &origins, const RouteProviderEngine::LocationsList &destinations) const {
        return RouteProviderEngine::getRouteInfoMatrix(origins, destinations);
    }

    Matrix<RouteDetails> LibOsrmEngine::getRouteDetailsMatrix(const RouteProviderEngine::LocationsList &origins, const RouteProviderEngine::LocationsList &destinations) const {
        return RouteProviderEngine::getRouteDetailsMatrix(origins, destinations);
    }

    TripInfo LibOsrmEngine::getTripInfo(const RouteProviderEngine::LocationsList &locations) const {
        return RouteProviderEngine::getTripInfo(locations);
    }

    TripDetails LibOsrmEngine::getTripDetails(const RouteProviderEngine::LocationsList &locations) const {
        return RouteProviderEngine::getTripDetails(locations);
    }
}