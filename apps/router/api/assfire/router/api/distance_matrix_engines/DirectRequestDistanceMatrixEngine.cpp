#include "DirectRequestDistanceMatrixEngine.hpp"
#include <fmt/ostream.h>
#include <assfire/locations/api/io/Streams.hpp>
#include <spdlog/spdlog.h>
#include <assert.h>
#include <algorithm>

using namespace assfire::router;

RouteInfo DirectRequestDistanceMatrixEngine::getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const {
    if (origin.getIndexTag() != matrix_tag || destination.getIndexTag() != matrix_tag) return getRouteInfo(origin.getRawLocation(), destination.getRawLocation());
    try {
        return engine->getSingleRouteInfo(origin.getRawLocation(), destination.getRawLocation());
    } catch (const std::exception &e) {
        return processError(origin.getRawLocation(), destination.getRawLocation(), e).getSummary();
    }
}

RouteDetails DirectRequestDistanceMatrixEngine::getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const {
    if (origin.getIndexTag() != matrix_tag || destination.getIndexTag() != matrix_tag) return getRouteDetails(origin.getRawLocation(), destination.getRawLocation());
    try {
        return engine->getSingleRouteDetails(origin.getRawLocation(), destination.getRawLocation());
    } catch (const std::exception &e) {
        return processError(origin.getRawLocation(), destination.getRawLocation(), e);
    }
}

IndexedLocation DirectRequestDistanceMatrixEngine::addLocation(const Location &location, LocationType type) {
    return IndexedLocation(0, matrix_tag, location);
}

RouteInfo DirectRequestDistanceMatrixEngine::getRouteInfo(const Location &origin, const Location &destination) const {
    try {
        return engine->getSingleRouteInfo(origin, destination);
    } catch (const std::exception &e) {
        return processError(origin, destination, e).getSummary();
    }
}

RouteDetails DirectRequestDistanceMatrixEngine::getRouteDetails(const Location &origin, const Location &destination) const {
    try {
        return engine->getSingleRouteDetails(origin, destination);
    } catch (const std::exception &e) {
        return processError(origin, destination, e);
    }
}

TripInfo DirectRequestDistanceMatrixEngine::getTripInfo(const DistanceMatrixEngine::LocationsList &locations) const {
    try {
        return engine->getTripInfo(locations);
    } catch (const std::exception &e) {
        return processTripInfoError(locations, e);
    }
}

TripDetails DirectRequestDistanceMatrixEngine::getTripDetails(const DistanceMatrixEngine::LocationsList &locations) const {
    try {
        return engine->getTripDetails(locations);
    } catch (const std::exception &e) {
        return processTripDetailsError(locations, e);
    }
}

TripInfo DirectRequestDistanceMatrixEngine::getTripInfo(const DistanceMatrixEngine::IndexedLocationsList &locations) const {
    LocationsList deindexed_locs;
    deindexed_locs.reserve(locations.size());
    std::transform(locations.begin(), locations.end(), std::back_inserter(deindexed_locs), [](const auto &l) { return l.getRawLocation(); });
    try {
        return engine->getTripInfo(deindexed_locs);
    } catch (const std::exception &e) {
        return processTripInfoError(deindexed_locs, e);
    }
}

TripDetails DirectRequestDistanceMatrixEngine::getTripDetails(const DistanceMatrixEngine::IndexedLocationsList &locations) const {
    LocationsList deindexed_locs;
    deindexed_locs.reserve(locations.size());
    std::transform(locations.begin(), locations.end(), std::back_inserter(deindexed_locs), [](const auto &l) { return l.getRawLocation(); });
    try {
        return engine->getTripDetails(deindexed_locs);
    } catch (const std::exception &e) {
        return processTripDetailsError(deindexed_locs, e);
    }
}

RouteDetails DirectRequestDistanceMatrixEngine::processError(const Location &from, const Location &to, const std::exception &e) const {
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

TripInfo DirectRequestDistanceMatrixEngine::processTripInfoError(const DistanceMatrixEngine::LocationsList &locations, const std::exception &e) const {
    switch (error_policy) {
        case DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY:
            SPDLOG_WARN("Error occurred when calculating routes vector for [{}:{}]: {}. Route will be set to INFINITY", *locations.begin(), *locations.rbegin(), e.what());
            return TripInfo::infinity();
        case DistanceMatrixErrorPolicy::ON_ERROR_THROW:
        default:
            throw e;
    }
}

TripDetails DirectRequestDistanceMatrixEngine::processTripDetailsError(const DistanceMatrixEngine::LocationsList &locations, const std::exception &e) const {
    switch (error_policy) {
        case DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY:
            SPDLOG_WARN("Error occurred when calculating routes vector for [{}:{}]: {}. Trip will be set to INFINITY", *locations.begin(), *locations.rbegin(), e.what());
            return TripDetails::infinity();
        case DistanceMatrixErrorPolicy::ON_ERROR_THROW:
        default:
            throw e;
    }
}

