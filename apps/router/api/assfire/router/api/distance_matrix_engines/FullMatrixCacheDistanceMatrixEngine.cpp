#include "FullMatrixCacheDistanceMatrixEngine.hpp"
#include <spdlog/spdlog.h>
#include <fmt/ostream.h>
#include "assfire/locations/api/io/Streams.hpp"
#include <assert.h>

using namespace assfire::router;

RouteInfo FullMatrixCacheDistanceMatrixEngine::getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const {
    if (origin.getIndexTag() != matrix_tag || destination.getIndexTag() != matrix_tag) return getRouteInfo(origin.getRawLocation(), destination.getRawLocation());
    return getCachedRouteInfo(origin.getId(), destination.getId());
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const {
    if (origin.getIndexTag() != matrix_tag || destination.getIndexTag() != matrix_tag) return getRouteDetails(origin.getRawLocation(), destination.getRawLocation());
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
    } catch (const std::exception &e) {
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
    } catch (const std::exception &e) {
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
    int known_origins_count = route_details_cache.getRowsCount();
    int known_destinations_count = route_details_cache.getColumnsCount();
    int new_origins_count = known_locations.size() - known_origins_count;
    int new_destinations_count = known_locations.size() - known_destinations_count;

    SPDLOG_DEBUG("Distance matrix cache is out of date. Rebuilding for {} new origins and {} new destinations", new_origins_count, new_destinations_count);

    Matrix<RouteDetails> new_bottom_submatrix(0, 0);
    Matrix<RouteDetails> new_right_submatrix(0, 0);
    if (new_origins_count > 0) {
        LocationsList new_origins;
        new_origins.reserve(new_origins_count);
        std::copy(known_locations.begin() + known_origins_count, known_locations.end(), std::back_inserter(new_origins));
        try {
            new_bottom_submatrix = engine->getRouteDetailsMatrix(new_origins, known_locations);
        } catch (const std::exception &e) {
            new_bottom_submatrix = processBatchError(new_origins, known_locations, e);
        }
    }
    if (new_destinations_count > 0 && known_origins_count > 0) {
        LocationsList new_destinations;
        new_destinations.reserve(new_destinations_count);
        std::copy(known_locations.begin() + known_origins_count, known_locations.end(), std::back_inserter(new_destinations));
        LocationsList known_origins;
        known_origins.reserve(known_origins_count);
        std::copy(known_locations.begin(), known_locations.begin() + known_origins_count, std::back_inserter(known_origins));
        try {
            new_right_submatrix = engine->getRouteDetailsMatrix(known_origins, new_destinations);
        } catch (const std::exception &e) {
            new_right_submatrix = processBatchError(known_origins, new_destinations, e);
        }
    }

    this->route_details_cache = Matrix<RouteDetails>(
            known_locations.size(),
            known_locations.size(),
            [&](int i, int j) {
                if (i < known_origins_count && j < known_destinations_count) { // Both locations are already in cache
                    SPDLOG_TRACE("Reusing cached value for locations {}, {}: {}", i, j, route_details_cache.at(i,j).getDistance().toMeters());
                    return route_details_cache.at(i, j);
                } else if (i < known_origins_count && j >= known_destinations_count) { // New destination -> take from right submatrix
                    SPDLOG_TRACE("Using right submatrix for {}, {}: {}", i, j, new_right_submatrix.at(i,j - known_destinations_count).getDistance().toMeters());
                    return new_right_submatrix.at(i, j - known_destinations_count);
                } else if (i >= known_origins_count && j < known_destinations_count) { // New origin -> take from bottom submatrix
                    SPDLOG_TRACE("Using bottom submatrix for {}, {}: {}", i, j, new_bottom_submatrix.at(i - known_origins_count, j).getDistance().toMeters());
                    return new_bottom_submatrix.at(i - known_origins_count, j);
                } else if (i >= known_origins_count && j >= known_destinations_count) { // New both origin and destination -> take from bottom submatrix
                    SPDLOG_TRACE("Using bottom+ submatrix for {}, {}: {}", i, j, new_bottom_submatrix.at(i - known_origins_count, j).getDistance().toMeters());
                    return new_bottom_submatrix.at(i - known_origins_count, j);
                } else {
                    SPDLOG_WARN("Single route mode is used for route calculation for locations {}->{}", known_locations.at(i), known_locations.at(j));
                    return engine->getSingleRouteDetails(known_locations.at(i), known_locations.at(j));
                }
            }
    );

    is_initialized = true;
}

FullMatrixCacheDistanceMatrixEngine::LocationKey FullMatrixCacheDistanceMatrixEngine::encodeLocation(const Location &location) {
    return std::to_string(location.getLatitude().encodedValue()) + std::to_string(location.getLongitude().encodedValue());
}

RouteInfo FullMatrixCacheDistanceMatrixEngine::getCachedRouteInfo(int origin_id, int destination_id) const {
    return getCachedRouteDetails(origin_id, destination_id).getSummary();
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::getCachedRouteDetails(int origin_id, int destination_id) const {
    if (!is_initialized) initialize();
    return route_details_cache.at(origin_id, destination_id);
}

RouteDetails FullMatrixCacheDistanceMatrixEngine::processError(const Location &from, const Location &to, const std::exception &e) const {
    switch (error_policy) {
        case DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY:
            SPDLOG_WARN("Error occurred when calculating route for {}->{}: {}. Route will be set to INFINITY",
                        from, to, e.what());
            return RouteDetails::infinity(from, to);
        case DistanceMatrixErrorPolicy::ON_ERROR_THROW:
        default:
            throw e;
    }
}

Matrix<RouteDetails> FullMatrixCacheDistanceMatrixEngine::processBatchError(const LocationsList &from, const LocationsList &to, const std::exception &e) const {
    switch (error_policy) {
        case DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY:
            SPDLOG_WARN("Error occurred when calculating route details matrix for [{}:{}]->[{}:{}] (batch size: {}): {}. Route will be set to INFINITY",
                        *from.begin(),
                        *from.rbegin(),
                        *to.begin(),
                        *to.rbegin(),
                        from.size() * to.size(),
                        e.what());
            return {from.size(), to.size(), [&](int i, int j) {
                return RouteDetails::infinity(from[i], to[j]);
            }};
        case DistanceMatrixErrorPolicy::ON_ERROR_THROW:
        default:
            throw e;
    }
}

TripInfo FullMatrixCacheDistanceMatrixEngine::processTripInfoError(const DistanceMatrixEngine::LocationsList &locations, const std::exception &e) const {
    switch (error_policy) {
        case DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY:
            SPDLOG_WARN("Error occurred when calculating routes vector for [{}:{}]: {}. Route will be set to INFINITY", *locations.begin(), *locations.rbegin(), e.what());
            return TripInfo::infinity();
        case DistanceMatrixErrorPolicy::ON_ERROR_THROW:
        default:
            throw e;
    }
}


TripDetails FullMatrixCacheDistanceMatrixEngine::processTripDetailsError(const DistanceMatrixEngine::LocationsList &locations, const std::exception &e) const {
    switch (error_policy) {
        case DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY:
            SPDLOG_WARN("Error occurred when calculating routes vector for [{}:{}]: {}. Trip will be set to INFINITY", *locations.begin(), *locations.rbegin(), e.what());
            return TripDetails::infinity();
        case DistanceMatrixErrorPolicy::ON_ERROR_THROW:
        default:
            throw e;
    }
}

TripInfo FullMatrixCacheDistanceMatrixEngine::getTripInfo(const DistanceMatrixEngine::LocationsList &locations) const {
    std::vector<int> found_locations;
    found_locations.reserve(locations.size());
    bool all_found = true;
    for (const Location &loc: locations) {
        auto iter = known_locations_mapping.find(encodeLocation(loc));
        found_locations.push_back(iter->second);
        if (iter == known_locations_mapping.end()) {
            all_found = false;
            break;
        }
    }

    if (all_found) {
        return getCachedTripInfo(found_locations);
    } else {
        try {
            return engine->getTripInfo(locations);
        } catch (const std::exception &e) {
            return processTripInfoError(locations, e);
        }
    }
}

TripDetails FullMatrixCacheDistanceMatrixEngine::getTripDetails(const DistanceMatrixEngine::LocationsList &locations) const {
    std::vector<int> found_locations;
    found_locations.reserve(locations.size());
    bool all_found = true;
    for (const Location &loc: locations) {
        auto iter = known_locations_mapping.find(encodeLocation(loc));
        found_locations.push_back(iter->second);
        if (iter == known_locations_mapping.end()) {
            all_found = false;
            break;
        }
    }

    if (all_found) {
        return getCachedTripDetails(found_locations);
    } else {
        try {
            return engine->getTripDetails(locations);
        } catch (const std::exception &e) {
            return processTripDetailsError(locations, e);
        }
    }
}

TripInfo FullMatrixCacheDistanceMatrixEngine::getTripInfo(const DistanceMatrixEngine::IndexedLocationsList &locations) const {
    std::vector<int> found_locations;
    found_locations.reserve(locations.size());
    bool all_found = true;
    for (const IndexedLocation &loc: locations) {
        if (loc.getIndexTag() == matrix_tag) {
            found_locations.push_back(loc.getId());
        } else {
            all_found = false;
            break;
        }
    }
    if (all_found) {
        return getCachedTripInfo(found_locations);
    } else {
        LocationsList deindexed_locs;
        deindexed_locs.reserve(locations.size());
        std::transform(locations.begin(), locations.end(), std::back_inserter(deindexed_locs), [](const auto &l) { return l.getRawLocation(); });
        try {
            return engine->getTripInfo(deindexed_locs);
        } catch (const std::exception &e) {
            return processTripInfoError(deindexed_locs, e);
        }
    }
}

TripDetails FullMatrixCacheDistanceMatrixEngine::getTripDetails(const DistanceMatrixEngine::IndexedLocationsList &locations) const {
    std::vector<int> found_locations;
    found_locations.reserve(locations.size());
    bool all_found = true;
    for (const IndexedLocation &loc: locations) {
        if (loc.getIndexTag() == matrix_tag) {
            found_locations.push_back(loc.getId());
        } else {
            all_found = false;
            break;
        }
    }
    if (all_found) {
        return getCachedTripDetails(found_locations);
    } else {
        LocationsList deindexed_locs;
        deindexed_locs.reserve(locations.size());
        std::transform(locations.begin(), locations.end(), std::back_inserter(deindexed_locs), [](const auto &l) { return l.getRawLocation(); });
        try {
            return engine->getTripDetails(deindexed_locs);
        } catch (const std::exception &e) {
            return processTripDetailsError(deindexed_locs, e);
        }
    }
}

TripInfo FullMatrixCacheDistanceMatrixEngine::getCachedTripInfo(const std::vector<int> &ids) const {
    TripInfo::RouteInfoList cached_infos;
    cached_infos.reserve(ids.size() - 1);
    Distance total_distance = Distance::zero();
    TimeInterval total_duration = TimeInterval::zero();
    for (int i = 0; i < ids.size() - 1; ++i) {
        RouteInfo route = getCachedRouteInfo(ids[i], ids[i + 1]);
        total_distance += route.getDistance();
        total_duration += route.getDuration();
        cached_infos.push_back(route);
    }
    return TripInfo(total_distance, total_duration, cached_infos);
}

TripDetails FullMatrixCacheDistanceMatrixEngine::getCachedTripDetails(const std::vector<int> &ids) const {
    TripDetails::RouteDetailsList cached_details;
    cached_details.reserve(ids.size() - 1);
    Distance total_distance = Distance::zero();
    TimeInterval total_duration = TimeInterval::zero();
    for (int i = 0; i < ids.size() - 1; ++i) {
        RouteDetails route = getCachedRouteDetails(ids[i], ids[i + 1]);
        total_distance += route.getDistance();
        total_duration += route.getDuration();
        cached_details.push_back(route);
    }
    return TripDetails(total_distance, total_duration, cached_details);
}
