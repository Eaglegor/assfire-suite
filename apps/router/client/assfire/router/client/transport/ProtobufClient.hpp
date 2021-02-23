#pragma once

#include <assfire/api/v1/service/router/router.grpc.pb.h>

namespace assfire::router {
    class ProtobufClient {
    public:
        using GetSingleRouteRequest = assfire::api::v1::service::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::service::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::service::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::service::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::service::router::ResponseStatus;

        using RoutesBatchConsumer = std::function<void(GetRoutesBatchResponse)>;
        using RequestSupplier = std::function<bool(GetRoutesBatchRequest&)>;

        virtual ~ProtobufClient() = default;

        virtual GetSingleRouteResponse getRoute(const GetSingleRouteRequest &) const = 0;
        virtual void getRoutesBatch(const GetRoutesBatchRequest &, const RoutesBatchConsumer &) const = 0;
        virtual void getRoutesBatch(const RequestSupplier&, const RoutesBatchConsumer &) const = 0;
    };
}