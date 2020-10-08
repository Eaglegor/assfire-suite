#pragma once

#include <assfire/api/v1/service/router/router.grpc.pb.h>
#include <grpcpp/channel.h>
#include <grpcpp/security/credentials.h>
#include <functional>

namespace assfire::router
{
    class GrpcClient
    {
    public:
        using Port = std::size_t;

        using GetSingleRouteRequest = assfire::api::v1::service::router::GetSingleRouteRequest;
        using GetSingleRouteResponse = assfire::api::v1::service::router::GetSingleRouteResponse;
        using GetRoutesBatchRequest = assfire::api::v1::service::router::GetRoutesBatchRequest;
        using GetRoutesBatchResponse = assfire::api::v1::service::router::GetRoutesBatchResponse;
        using ResponseStatus = assfire::api::v1::service::router::ResponseStatus;

        using RoutesBatchConsumer = std::function<void(GetRoutesBatchResponse)>;
        using RequestSupplier = std::function<bool(GetRoutesBatchRequest&)>;

        GrpcClient(const std::string &server_host, Port server_port, bool use_ssl);

        [[nodiscard]]
        GetSingleRouteResponse getRoute(const GetSingleRouteRequest &) const;
        void getRoutesBatch(const GetRoutesBatchRequest &, const RoutesBatchConsumer &) const;
        void getRoutesBatch(const RequestSupplier&, const RoutesBatchConsumer &) const;

    private:
        std::shared_ptr<grpc::ChannelInterface> channel;
        std::unique_ptr<assfire::api::v1::service::router::RouterService::Stub> stub;
    };
}