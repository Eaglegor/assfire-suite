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
    private:
        const GrpcClient& transport_client;
    };
}