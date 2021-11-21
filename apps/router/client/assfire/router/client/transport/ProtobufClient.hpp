#pragma once

#include <assfire/api/v1/router/service.grpc.pb.h>

namespace assfire::router {
    class ProtobufClient {
    public:
        using GetSingleRouteRequest = assfire::api::v1::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::router::ResponseStatus;

        using RoutesBatchConsumer = std::function<void(GetRoutesBatchResponse)>;
        using RequestSupplier = std::function<bool(GetRoutesBatchRequest&)>;

        virtual ~ProtobufClient() = default;

        virtual GetSingleRouteResponse getRoute(const GetSingleRouteRequest &) const = 0;
        virtual void getRoutesBatch(const GetRoutesBatchRequest &, const RoutesBatchConsumer &) const = 0;
        virtual void getRoutesBatch(const RequestSupplier&, const RoutesBatchConsumer &) const = 0;
    };
}