#pragma once
#include <memory>
#include <assfire/api/v1/service/router/service.pb.h>

namespace assfire::router
{
    class RequestStrategy;

    class DistanceMatrix
    {
    public:
        using GetSingleRouteRequest = assfire::api::v1::services::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::services::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::services::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::services::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::services::router::ResponseStatus;

        using RoutesBatchConsumer = std::function<void(GetRoutesBatchResponse)>;
        using RequestSupplier = std::function<bool(GetRoutesBatchRequest&)>;

        DistanceMatrix(std::unique_ptr<RequestStrategy> &&request_strategy);

        void prepareRoute(const GetSingleRouteRequest &);
        void prepareRoutes(const GetRoutesBatchRequest &);

        [[nodiscard]]
        GetSingleRouteResponse getRoute(const GetSingleRouteRequest &) const;
        void getRoutesBatch(const GetRoutesBatchRequest &, const RoutesBatchConsumer &) const;
        void getRoutesBatch(const RequestSupplier&, const RoutesBatchConsumer &) const;

    private:
        std::unique_ptr<RequestStrategy> request_strategy;
    };
}