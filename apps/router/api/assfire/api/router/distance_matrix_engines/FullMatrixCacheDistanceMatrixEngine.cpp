#include "FullMatrixCacheDistanceMatrixEngine.hpp"
#include <assert.h>

using namespace assfire::router;

RouteInfo FullMatrixCacheDistanceMatrixEngine::getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const {
    assert(origin.getIndexTag() == matrix_tag && destination.getIndexTag() == matrix_tag);
    return getCachedRouteInfo(origin.getId(), destination.getId());
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const {
    assert(origin.getIndexTag() == matrix_tag && destination.getIndexTag() == matrix_tag);
    return getCachedRouteDetails(origin.getId(), destination.getId());
}

RouteInfo FullMatrixCacheDistanceMatrixEngine::getRouteInfo(const Location &origin, const Location &destination) const {
    auto origin_iter = known_locations_mapping.find(encodeLocation(origin));
    auto destination_iter = known_locations_mapping.find(encodeLocation(destination));
    if (origin_iter != known_locations_mapping.end() && destination_iter != known_locations_mapping.end()) {
        return getCachedRouteInfo(origin_iter->second, destination_iter->second);
    }
    return engine->getSingleRouteInfo(origin, destination);
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getRouteDetails(const Location &origin, const Location &destination) const {
    auto origin_iter = known_locations_mapping.find(encodeLocation(origin));
    auto destination_iter = known_locations_mapping.find(encodeLocation(destination));
    if (origin_iter != known_locations_mapping.end() && destination_iter != known_locations_mapping.end()) {
        return getCachedRouteDetails(origin_iter->second, destination_iter->second);
    }
    return engine->getSingleRouteDetails(origin, destination);
}

IndexedLocation FullMatrixCacheDistanceMatrixEngine::addLocation(const Location &location, LocationType type) {
    std::lock_guard<std::mutex> guard(cache_update_lock);
    initialized = false;
    known_locations.push_back(location);
    int index = known_locations.size() - 1;
    known_locations_mapping.insert_or_assign(encodeLocation(location), index);
    return IndexedLocation(index, matrix_tag, location);
}

void FullMatrixCacheDistanceMatrixEngine::initialize() const {
    std::lock_guard<std::mutex> guard(cache_update_lock);
    if (initialized) return;
    std::unique_ptr<Matrix<RouteDetails>> new_cache = std::make_unique<Matrix<RouteDetails>>(known_locations.size(), known_locations.size(), [&](int i, int j) {
        if (route_details_cache && route_details_cache->getRowsCount() > i && route_details_cache->getColumnsCount() > j) {
            return route_details_cache->at(i, j);
        } else {
            return engine->getSingleRouteDetails(known_locations.at(i), known_locations.at(j));
        }
    });

    route_details_cache.swap(new_cache);
    initialized = true;
}

std::string FullMatrixCacheDistanceMatrixEngine::encodeLocation(const Location &location) const {
    return std::to_string(location.getLatitude().encodedValue()) + std::to_string(location.getLatitude().encodedValue());
}

RouteInfo FullMatrixCacheDistanceMatrixEngine::getCachedRouteInfo(int origin_id, int destination_id) const {
    if (!initialized) initialize();
    return route_details_cache->at(origin_id, destination_id).getSummary();
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getCachedRouteDetails(int origin_id, int destination_id) const {
    if (!initialized) initialize();
    return route_details_cache->at(origin_id, destination_id);
}
