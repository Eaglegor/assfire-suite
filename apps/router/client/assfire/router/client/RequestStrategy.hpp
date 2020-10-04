#pragma once

#include <assfire/api/v1/service/router/service.pb.h>

namespace assfire::router
{
    class RequestStrategy {
    public:
        using GetSingleRouteRequest = assfire::api::v1::services::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::services::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::services::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::services::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::services::router::ResponseStatus;

        using RoutesBatchConsumer = std::function<void(GetRoutesBatchResponse)>;
        using RequestSupplier = std::function<bool(GetRoutesBatchRequest&)>;

        virtual void prepareRoute(const GetSingleRouteRequest &) = 0;
        virtual void prepareRoutes(const GetRoutesBatchRequest &) = 0;

        [[nodiscard]]
        virtual GetSingleRouteResponse getRoute(const GetSingleRouteRequest &) const = 0;
        virtual void getRoutesBatch(const GetRoutesBatchRequest &, const RoutesBatchConsumer &) const = 0;
        virtual void getRoutesBatch(const RequestSupplier&, const RoutesBatchConsumer &) const = 0;
    };
}