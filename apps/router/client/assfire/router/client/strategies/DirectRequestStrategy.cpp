#include "DirectRequestStrategy.hpp"
#include "assfire/router/client/transport/GrpcClient.hpp"
#include <spdlog/spdlog.h>

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

RequestStrategy::LocationId DirectRequestStrategy::addLocation(const RequestStrategy::Location &location)
{
    locations.push_back(location);
    return locations.size() - 1;
}

RequestStrategy::RoutingOptionsId DirectRequestStrategy::addRoutingOptions(const RequestStrategy::RoutingOptions &routing_options)
{
    this->routing_options.push_back(routing_options);
    return this->routing_options.size() - 1;
}

void DirectRequestStrategy::prepareRoute(const RequestStrategy::Location &origin, const RequestStrategy::Location &destination, const RequestStrategy::RoutingOptions &options)
{
    // Do nothing
}

void DirectRequestStrategy::prepareRoute(RequestStrategy::LocationId origin, RequestStrategy::LocationId destination, RequestStrategy::RoutingOptionsId options)
{
    // Do nothing
}

RequestStrategy::RouteInfo DirectRequestStrategy::getRoute(const RequestStrategy::Location &origin, const RequestStrategy::Location &destination, const RequestStrategy::RoutingOptions &options) const
{
    GetSingleRouteRequest request;
    request.mutable_origin()->CopyFrom(origin);
    request.mutable_destination()->CopyFrom(destination);
    request.mutable_options()->CopyFrom(options);
    return getRoute(request).route_info();
}

RequestStrategy::RouteInfo DirectRequestStrategy::getRoute(RequestStrategy::LocationId origin, RequestStrategy::LocationId destination, RequestStrategy::RoutingOptionsId options)
{
    if(origin >= locations.size() || destination >= locations.size() || options >= routing_options.size()) {
        SPDLOG_ERROR("Invalid location or options index: {}->{} ({})", origin, destination, options);
        throw std::invalid_argument("Invalid location or options index");
    }
    return getRoute(locations[origin], locations[destination], routing_options[options]);
}

