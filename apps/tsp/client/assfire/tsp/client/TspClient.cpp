#include "TspClient.hpp"
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <spdlog/spdlog.h>
#include <chrono>

using namespace assfire::tsp;

namespace {
    constexpr int DEFAULT_TIMEOUT_MS = 60000;
}

TspClient::TspClient(const std::string &host, int port, bool use_ssl)
{
    std::shared_ptr<grpc::ChannelCredentials> credentials;
    if (use_ssl) {
        credentials = grpc::SslCredentials(grpc::SslCredentialsOptions());
    } else {
        credentials = grpc::InsecureChannelCredentials();
    }

    channel = grpc::CreateChannel(host + ": " + std::to_string(port), credentials);
    stub = assfire::api::v1::service::tsp::TspService::NewStub(channel);
}

TspClient::SolveTspSyncResponse TspClient::solveTsp(const TspClient::SolveTspSyncRequest &request) const
{
    grpc::ClientContext client_context;
    client_context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(DEFAULT_TIMEOUT_MS));
    TspClient::SolveTspSyncResponse response;
    grpc::Status status = stub->SolveTspSync(&client_context, request, &response);

    if(!status.ok()) {
        SPDLOG_ERROR("Error while processing gRPC call: {}", status.error_message());
        throw std::runtime_error(status.error_message());
    }

    return response;
}

void TspClient::solveTsp(const TspClient::SolveTspStreamingRequest &request, const TspClient::StreamingResponseConsumer &response_consumer) const
{
    grpc::ClientContext client_context;
    SolveTspStreamingResponse response;

    std::unique_ptr<grpc::ClientReader<SolveTspStreamingResponse>> reader = stub->SolveTspStreaming(&client_context, request);
    while (reader->Read(&response)) {
        response_consumer(response);
    }

    grpc::Status result = reader->Finish();

    if (!result.ok()) {
        SPDLOG_ERROR("Error while processing gRPC call: {}", result.error_message());
        throw std::runtime_error(result.error_message());
    }
}
