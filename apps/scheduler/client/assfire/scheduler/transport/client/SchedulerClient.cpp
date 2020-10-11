#include "SchedulerClient.hpp"
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <spdlog/spdlog.h>
#include <chrono>

using namespace assfire::scheduler::transport;

namespace
{
    constexpr int DEFAULT_TIMEOUT_MS = 60000;
}

SchedulerClient::SchedulerClient(const std::string &host, int port, bool use_ssl)
{
    std::shared_ptr<grpc::ChannelCredentials> credentials;
    if (use_ssl) {
        credentials = grpc::SslCredentials(grpc::SslCredentialsOptions());
    } else {
        credentials = grpc::InsecureChannelCredentials();
    }

    channel = grpc::CreateChannel(host + ": " + std::to_string(port), credentials);
    stub = assfire::api::v1::service::scheduler::transport::SchedulerService::NewStub(channel);
}

SchedulerClient::BuildScheduleResponse SchedulerClient::buildSchedule(const SchedulerClient::BuildScheduleRequest &request) const
{
    grpc::ClientContext client_context;
    client_context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(DEFAULT_TIMEOUT_MS));
    BuildScheduleResponse response;
    grpc::Status status = stub->BuildSchedule(&client_context, request, &response);

    if(!status.ok()) {
        SPDLOG_ERROR("Error while processing gRPC call: {}", status.error_message());
        throw std::runtime_error(status.error_message());
    }

    return response;
}
