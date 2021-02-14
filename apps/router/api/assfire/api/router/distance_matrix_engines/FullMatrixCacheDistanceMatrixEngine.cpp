#include "FullMatrixCacheDistanceMatrixEngine.hpp"
#include <assert.h>

using namespace assfire::router;

RouteInfo FullMatrixCacheDistanceMatrixEngine::getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const {
    assert(origin.getIndexTag() == matrix_tag && destination.getIndexTag() == matrix_tag);
    if (!initialized) initialize();
    return route_details_cache->at(origin.getId(), destination.getId()).getSummary();
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const {
    assert(origin.getIndexTag() == matrix_tag && destination.getIndexTag() == matrix_tag);
    if (!initialized) initialize();
    return route_details_cache->at(origin.getId(), destination.getId());
}

RouteInfo FullMatrixCacheDistanceMatrixEngine::getRouteInfo(const Location &origin, const Location &destination) const {
    return engine->getSingleRouteInfo(origin, destination);
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getRouteDetails(const Location &origin, const Location &destination) const {
    return engine->getSingleRouteDetails(origin, destination);
}

IndexedLocation FullMatrixCacheDistanceMatrixEngine::addLocation(const Location &location) {
    initialized = false;
    known_locations.push_back(location);
    return IndexedLocation(known_locations.size() - 1, matrix_tag, location);
}

void FullMatrixCacheDistanceMatrixEngine::initialize() const {
    std::unique_ptr<Matrix<RouteDetails>> new_cache = std::make_unique<Matrix<RouteDetails>>(known_locations.size(), known_locations.size(), [&](int i, int j) {
        if (route_details_cache && route_details_cache->getRowsCount() > i && route_details_cache->getColumnsCount() > j) {
            return route_details_cache->at(i, j);
        } else {
            return getRouteDetails(known_locations.at(i), known_locations.at(j));
        }
    });

    route_details_cache.swap(new_cache);
}
