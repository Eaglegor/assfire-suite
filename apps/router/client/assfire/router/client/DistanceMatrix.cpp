#include "DistanceMatrix.hpp"
#include "assfire/router/client/RequestStrategy.hpp"

using namespace assfire::router;

DistanceMatrix::DistanceMatrix(std::unique_ptr<RequestStrategy> &&request_strategy)
        : request_strategy(std::move(request_strategy))
{
}

void DistanceMatrix::prepareRoute(const DistanceMatrix::GetSingleRouteRequest &request)
{
    request_strategy->prepareRoute(request);
}

void DistanceMatrix::prepareRoutes(const DistanceMatrix::GetRoutesBatchRequest &request)
{
    request_strategy->prepareRoutes(request);
}

DistanceMatrix::GetSingleRouteResponse DistanceMatrix::getRoute(const GetSingleRouteRequest &request) const
{
    return request_strategy->getRoute(request);
}

void DistanceMatrix::getRoutesBatch(const GetRoutesBatchRequest &request, const RoutesBatchConsumer &consumer) const
{
    request_strategy->getRoutesBatch(request, consumer);
}

void DistanceMatrix::getRoutesBatch(const RequestSupplier &supplier, const RoutesBatchConsumer &consumer) const
{
    request_strategy->getRoutesBatch(supplier, consumer);
}

DistanceMatrix::RouteInfo DistanceMatrix::getRoute(const DistanceMatrix::Location &origin, const DistanceMatrix::Location &destination, const DistanceMatrix::RoutingOptions &options) const
{
    return request_strategy->getRoute(origin, destination, options);
}

DistanceMatrix::LocationId DistanceMatrix::addLocation(const DistanceMatrix::Location &location)
{
    return request_strategy->addLocation(location);
}

DistanceMatrix::RoutingOptionsId DistanceMatrix::addRoutingOptions(const DistanceMatrix::RoutingOptions &routing_options)
{
    return request_strategy->addRoutingOptions(routing_options);
}

void DistanceMatrix::prepareRoute(const DistanceMatrix::Location &origin, const DistanceMatrix::Location &destination, const DistanceMatrix::RoutingOptions &options)
{
    return request_strategy->prepareRoute(origin, destination, options);
}

void DistanceMatrix::prepareRoute(DistanceMatrix::LocationId origin, DistanceMatrix::LocationId destination, DistanceMatrix::RoutingOptionsId options)
{
    return request_strategy->prepareRoute(origin, destination, options);
}

DistanceMatrix::RouteInfo DistanceMatrix::getRoute(DistanceMatrix::LocationId origin, DistanceMatrix::LocationId destination, DistanceMatrix::RoutingOptionsId options)
{
    return request_strategy->getRoute(origin, destination, options);
}
