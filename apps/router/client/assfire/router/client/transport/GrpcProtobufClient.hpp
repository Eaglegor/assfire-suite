#pragma once

#include "ProtobufClient.hpp"
#include <grpcpp/channel.h>
#include <grpcpp/security/credentials.h>
#include <functional>

namespace assfire::router
{
    class GrpcProtobufClient : public ProtobufClient
    {
    public:
        using Port = std::size_t;

        GrpcProtobufClient(const std::string &server_host, Port server_port, bool use_ssl);

        GetSingleRouteResponse getRoute(const GetSingleRouteRequest &) const override;
        void getRoutesBatch(const GetRoutesBatchRequest &, const RoutesBatchConsumer &) const override;
        void getRoutesBatch(const RequestSupplier&, const RoutesBatchConsumer &) const override;

    private:
        std::shared_ptr<grpc::ChannelInterface> channel;
        std::unique_ptr<assfire::api::v1::router::RouterService::Stub> stub;
    };
}