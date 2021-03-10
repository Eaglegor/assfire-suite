#include "DirectRequestDistanceMatrixEngine.hpp"
#include <spdlog/spdlog.h>
#include <assert.h>

using namespace assfire::router;

RouteInfo DirectRequestDistanceMatrixEngine::getRouteInfo(const IndexedLocation &origin, const IndexedLocation &destination) const {
    if(origin.getIndexTag() != matrix_tag || destination.getIndexTag() != matrix_tag) return getRouteInfo(origin.getRawLocation(), destination.getRawLocation());
    try {
        return engine->getSingleRouteInfo(origin.getRawLocation(), destination.getRawLocation());
    } catch (const std::exception& e) {
        return processError(origin.getRawLocation(), destination.getRawLocation(), e).getSummary();
    }
}

RouteDetails DirectRequestDistanceMatrixEngine::getRouteDetails(const IndexedLocation &origin, const IndexedLocation &destination) const {
    if(origin.getIndexTag() != matrix_tag || destination.getIndexTag() != matrix_tag) return getRouteDetails(origin.getRawLocation(), destination.getRawLocation());
    try {
        return engine->getSingleRouteDetails(origin.getRawLocation(), destination.getRawLocation());
    } catch(const std::exception& e) {
        return processError(origin.getRawLocation(), destination.getRawLocation(), e);
    }
}

IndexedLocation DirectRequestDistanceMatrixEngine::addLocation(const Location &location, LocationType type) {
    return IndexedLocation(0, matrix_tag, location);
}

RouteInfo DirectRequestDistanceMatrixEngine::getRouteInfo(const Location &origin, const Location &destination) const {
    try {
        return engine->getSingleRouteInfo(origin, destination);
    } catch(const std::exception& e) {
        return processError(origin, destination, e).getSummary();
    }
}

RouteDetails DirectRequestDistanceMatrixEngine::getRouteDetails(const Location &origin, const Location &destination) const {
    try {
        return engine->getSingleRouteDetails(origin, destination);
    } catch(const std::exception& e) {
        return processError(origin, destination, e);
    }
}

RouteDetails DirectRequestDistanceMatrixEngine::processError(const Location& from, const Location& to, const std::exception &e) const {
    switch(error_policy) {
        case DistanceMatrixErrorPolicy::ON_ERROR_RETURN_INFINITY:
            SPDLOG_WARN("Error occurred when calculating route for ({},{})->({},{}): {}. Route will be set to INFINITY",
                        from.getLatitude().doubleValue(), from.getLongitude().doubleValue(),
                        to.getLatitude().doubleValue(), to.getLongitude().doubleValue(),
                        e.what());
            return RouteDetails::infinity(from, to);
        case DistanceMatrixErrorPolicy::ON_ERROR_THROW:
        default:
            throw e;
    }
}
