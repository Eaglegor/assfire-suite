#include "DirectRequestDistanceMatrixEngine.hpp"
#include <assert.h>

using namespace assfire::router;

RouteInfo DirectRequestDistanceMatrixEngine::getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const {
    assert(origin.getIndexTag() == matrix_tag && destination.getIndexTag() == matrix_tag);
    return engine->getSingleRouteInfo(origin.getLocation(), destination.getLocation());
}

RouteDetails DirectRequestDistanceMatrixEngine::getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const {
    assert(origin.getIndexTag() == matrix_tag && destination.getIndexTag() == matrix_tag);
    return engine->getSingleRouteDetails(origin.getLocation(), destination.getLocation());
}

IndexedLocation DirectRequestDistanceMatrixEngine::addLocation(const Location &location, LocationType type) {
    known_locations.push_back(location);
    return IndexedLocation(known_locations.size() - 1, matrix_tag, location);
}

RouteInfo DirectRequestDistanceMatrixEngine::getRouteInfo(const Location &origin, const Location &destination) const {
    return engine->getSingleRouteInfo(origin, destination);
}

RouteDetails DirectRequestDistanceMatrixEngine::getRouteDetails(const Location &origin, const Location &destination) const {
    return engine->getSingleRouteDetails(origin, destination);
}
