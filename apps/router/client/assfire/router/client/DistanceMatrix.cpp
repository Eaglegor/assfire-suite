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
