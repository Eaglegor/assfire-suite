#pragma once

#include <assfire/api/v1/service/sample/service.grpc.pb.h>
#include <grpcpp/channel.h>

namespace assfire::sample {
    class GreeterClient
    {
    public:
        GreeterClient(const std::string &host, int port, bool use_ssl);

        assfire::api::v1::services::sample::GreetResponse greet(const assfire::api::v1::services::sample::GreetRequest &) const;

    private:
        std::shared_ptr<grpc::ChannelInterface> channel;
        std::unique_ptr<assfire::api::v1::services::sample::GreeterService::Stub> stub;
    };
}