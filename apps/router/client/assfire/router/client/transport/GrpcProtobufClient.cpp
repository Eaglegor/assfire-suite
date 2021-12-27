#include "GrpcProtobufClient.hpp"
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <spdlog/spdlog.h>
#include <future>

using namespace assfire::router;

GrpcProtobufClient::GrpcProtobufClient(const std::string &server_host, Port server_port, bool use_ssl) {
    std::shared_ptr<grpc::ChannelCredentials> credentials;
    if (use_ssl) {
        credentials = grpc::SslCredentials(grpc::SslCredentialsOptions());
    } else {
        credentials = grpc::InsecureChannelCredentials();
    }
    SPDLOG_INFO("Creating router grpc channel for {}:{}", server_host, server_port);

    channel = grpc::CreateChannel(server_host + ":" + std::to_string(server_port), credentials);
    stub = assfire::api::v1::router::RouterService::NewStub(channel);
}

GrpcProtobufClient::GetSingleRouteResponse GrpcProtobufClient::getRoute(const GetSingleRouteRequest &request) const {
    grpc::ClientContext client_context;
    GetSingleRouteResponse response;

    grpc::Status result = stub->GetSingleRoute(&client_context, request, &response);

    if (!result.ok()) {
        SPDLOG_ERROR("Error while processing getRoute() gRPC call: {}", result.error_message());
        throw std::runtime_error(result.error_message());
    }

    return response;
}

void GrpcProtobufClient::getRoutesBatch(const GetRoutesBatchRequest &request, const RoutesBatchConsumer &consumer) const {
    grpc::ClientContext client_context;
    GetRoutesBatchResponse response;

    std::unique_ptr<grpc::ClientReader<GetRoutesBatchResponse>> reader = stub->GetRoutesBatch(&client_context, request);
    while (reader->Read(&response)) {
        consumer(response);
    }

    grpc::Status result = reader->Finish();

    if (!result.ok()) {
        SPDLOG_ERROR("Error while processing getRoutesBatch() gRPC call: {}", result.error_message());
        throw std::runtime_error(result.error_message());
    }
}

void GrpcProtobufClient::getRoutesBatch(const RequestSupplier &request_supplier, const RoutesBatchConsumer &consumer) const {
    grpc::ClientContext client_context;
    GetRoutesBatchResponse response;

    std::unique_ptr<grpc::ClientReaderWriter<GetRoutesBatchRequest, GetRoutesBatchResponse>> reader_writer = stub->GetStreamingRoutesBatch(&client_context);
    std::future<void> f = std::async(std::launch::async, [&]() {
        GetRoutesBatchRequest request;
        while (request_supplier(request) && reader_writer->Write(request));
        reader_writer->WritesDone();
    });
    while (reader_writer->Read(&response)) {
        consumer(response);
    }

    grpc::Status result = reader_writer->Finish();

    if (!result.ok()) {
        SPDLOG_ERROR("Error while processing getRoutesBatch() gRPC call: {}", result.error_message());
        throw std::logic_error(result.error_message());
    }
}
