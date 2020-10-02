#include "DirectRequestStrategy.hpp"
#include "assfire/router/client/transport/GrpcClient.hpp"

using namespace assfire::router;

DirectRequestStrategy::DirectRequestStrategy(const GrpcClient &transport_client)
        : transport_client(transport_client)
{

}

void DirectRequestStrategy::prepareRoute(const GetSingleRouteRequest &request)
{
    // Do nothing
}

void DirectRequestStrategy::prepareRoutes(const GetRoutesBatchRequest &request)
{
    // Do nothing
}

RequestStrategy::GetSingleRouteResponse DirectRequestStrategy::getRoute(const GetSingleRouteRequest &request) const
{
    return transport_client.getRoute(request);
}

void DirectRequestStrategy::getRoutesBatch(const GetRoutesBatchRequest &request, const RoutesBatchConsumer &consumer) const
{
    transport_client.getRoutesBatch(request, consumer);
}

void DirectRequestStrategy::getRoutesBatch(const RequestSupplier &supplier, const RoutesBatchConsumer &consumer) const
{
    transport_client.getRoutesBatch(supplier, consumer);
}

