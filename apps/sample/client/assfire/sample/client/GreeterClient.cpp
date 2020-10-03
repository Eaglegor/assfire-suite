#include "GreeterClient.hpp"
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

namespace assfire::sample {
    GreeterClient::GreeterClient(const std::string &host, int port, bool use_ssl)
    {
        std::shared_ptr<grpc::ChannelCredentials> credentials;
        if (use_ssl) {
            credentials = grpc::SslCredentials(grpc::SslCredentialsOptions());
        } else {
            credentials = grpc::InsecureChannelCredentials();
        }

        channel = grpc::CreateChannel(host + ": " + std::to_string(port), credentials);
        stub = assfire::api::v1::services::sample::GreeterService::NewStub(channel);
    }

    assfire::api::v1::services::sample::GreetResponse GreeterClient::greet(const assfire::api::v1::services::sample::GreetRequest &request) const
    {
        grpc::ClientContext client_context;
        assfire::api::v1::services::sample::GreetResponse response;
        grpc::Status result = stub->Greet(&client_context, request, &response);

        if (!result.ok()) {
            throw std::logic_error(result.error_message());
        }

        return response;
    }
}