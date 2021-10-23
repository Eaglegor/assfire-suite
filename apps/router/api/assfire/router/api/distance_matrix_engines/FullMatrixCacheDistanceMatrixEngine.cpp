#include "FullMatrixCacheDistanceMatrixEngine.hpp"
#include <spdlog/spdlog.h>
#include <assert.h>

using namespace assfire::router;

RouteInfo FullMatrixCacheDistanceMatrixEngine::getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const {
    if(origin.getIndexTag() != matrix_tag || destination.getIndexTag() != matrix_tag) return getRouteInfo(origin.getRawLocation(), destination.getRawLocation());
    return getCachedRouteInfo(origin.getId(), destination.getId());
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const {
    if(origin.getIndexTag() != matrix_tag || destination.getIndexTag() != matrix_tag) return getRouteDetails(origin.getRawLocation(), destination.getRawLocation());
    return getCachedRouteDetails(origin.getId(), destination.getId());
}

RouteInfo FullMatrixCacheDistanceMatrixEngine::getRouteInfo(const Location &origin, const Location &destination) const {
    auto origin_iter = known_locations_mapping.find(encodeLocation(origin));
    auto destination_iter = known_locations_mapping.find(encodeLocation(destination));
    if (origin_iter != known_locations_mapping.end() && destination_iter != known_locations_mapping.end()) {
        return getCachedRouteInfo(origin_iter->second, destination_iter->second);
    }
    try {
        return engine->getSingleRouteInfo(origin, destination);
    } catch(const std::exception& e) {
        return processError(origin, destination, e).getSummary();
    }
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getRouteDetails(const Location &origin, const Location &destination) const {
    auto origin_iter = known_locations_mapping.find(encodeLocation(origin));
    auto destination_iter = known_locations_mapping.find(encodeLocation(destination));
    if (origin_iter != known_locations_mapping.end() && destination_iter != known_locations_mapping.end()) {
        return getCachedRouteDetails(origin_iter->second, destination_iter->second);
    }
    try {
        return engine->getSingleRouteDetails(origin, destination);
    } catch(const std::exception& e) {
        return processError(origin, destination, e);
    }
}

IndexedLocation FullMatrixCacheDistanceMatrixEngine::addLocation(const Location &location, LocationType type) {
    std::lock_guard<std::mutex> guard(cache_update_lock);
    is_initialized = false;
    auto known_iter = known_locations_mapping.find(encodeLocation(location));
    if (known_iter != known_locations_mapping.end()) {
        return IndexedLocation(known_iter->second, matrix_tag, location);
    }
    known_locations.push_back(location);
    int index = known_locations.size() - 1;
    known_locations_mapping.insert_or_assign(encodeLocation(location), index);
    return IndexedLocation(index, matrix_tag, location);
}

void FullMatrixCacheDistanceMatrixEngine::initialize() const {
    std::lock_guard<std::mutex> guard(cache_update_lock);
    if (is_initialized) return;
    std::unique_ptr<Matrix<RouteDetails>> new_cache = std::make_unique<Matrix<RouteDetails>>(known_locations.size(), known_locations.size(), [&](int i, int j) {
        if (route_details_cache && route_details_cache->getRowsCount() > i && route_details_cache->getColumnsCount() > j) {
            return route_details_cache->at(i, j);
        } else {
            if (i == j) {
                return RouteDetails::zero(known_locations.at(i), known_locations.at(j));
            } else {
                try {
                    return engine->getSingleRouteDetails(known_locations.at(i), known_locations.at(j));
                } catch (const std::exception &e) {
                    return processError(known_locations.at(i), known_locations.at(j), e);
                }
            }
        }
    });

    route_details_cache.swap(new_cache);
    is_initialized = true;
}

std::string FullMatrixCacheDistanceMatrixEngine::encodeLocation(const Location &location) const {
    return std::to_string(location.getLatitude().encodedValue()) + std::to_string(location.getLongitude().encodedValue());
}

RouteInfo FullMatrixCacheDistanceMatrixEngine::getCachedRouteInfo(int origin_id, int destination_id) const {
    return getCachedRouteDetails(origin_id, destination_id).getSummary();
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getCachedRouteDetails(int origin_id, int destination_id) const {
    if (!is_initialized) initialize();
    return route_details_cache->at(origin_id, destination_id);
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::processError(const Location& from, const Location& to, const std::exception &e) const {
    switch(error_policy) {
        case DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY:
            SPDLOG_WARN("Error occurred when initializing cached route for ({},{})->({},{}): {}. Route will be set to INFINITY",
                        from.getLatitude().doubleValue(), from.getLongitude().doubleValue(),
                        to.getLatitude().doubleValue(), to.getLongitude().doubleValue(),
                        e.what());
            return RouteDetails::infinity(from, to);
        case DistanceMatrixErrorPolicy::ON_ERROR_THROW:
        default:
            throw e;
    }
}