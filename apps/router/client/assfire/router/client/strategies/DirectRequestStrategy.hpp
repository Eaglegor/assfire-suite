#pragma once

#include "assfire/router/client/RequestStrategy.hpp"

namespace assfire::router
{
    class GrpcClient;

    class DirectRequestStrategy : public RequestStrategy
    {
    public:
        explicit DirectRequestStrategy(const GrpcClient &transport_client);

        void prepareRoute(const GetSingleRouteRequest &request) override;

        void prepareRoutes(const GetRoutesBatchRequest &request) override;

        [[nodiscard]]
        GetSingleRouteResponse getRoute(const GetSingleRouteRequest &request) const override;

        void getRoutesBatch(const GetRoutesBatchRequest &request, const RoutesBatchConsumer &consumer) const override;

        void getRoutesBatch(const RequestSupplier &supplier, const RoutesBatchConsumer &consumer) const override;

        LocationId addLocation(const Location &location) override;

        RoutingOptionsId addRoutingOptions(const RoutingOptions &routing_options) override;

        void prepareRoute(const Location &origin, const Location &destination, const RoutingOptions &options) override;

        void prepareRoute(LocationId origin, LocationId destination, RoutingOptionsId options) override;

        RouteInfo getRoute(const Location &origin, const Location &destination, const RoutingOptions &options) const override;

        RouteInfo getRoute(LocationId origin, LocationId destination, RoutingOptionsId options) override;

    private:
        const GrpcClient& transport_client;
        std::vector<Location> locations;
        std::vector<RoutingOptions> routing_options;
    };
}