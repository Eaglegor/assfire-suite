#pragma once

#include <assfire/api/v1/service/tsp/tsp.grpc.pb.h>
#include <grpcpp/channel.h>
#include <functional>

namespace assfire::tsp {
    class TspClient
    {
    public:
        TspClient(const std::string &host, int port, bool use_ssl);

        using SolveTspSyncRequest = assfire::api::v1::service::tsp::SolveTspSyncRequest;
        using SolveTspSyncResponse = assfire::api::v1::service::tsp::SolveTspSyncResponse;
        using SolveTspStreamingRequest = assfire::api::v1::service::tsp::SolveTspStreamingRequest;
        using SolveTspStreamingResponse = assfire::api::v1::service::tsp::SolveTspStreamingResponse;

        using StreamingResponseConsumer = std::function<void(const SolveTspStreamingResponse&)>;

        /**
         * Solves TSP and returns result. Blocks until result is ready or timeout is exceeded
         */
        SolveTspSyncResponse solveTsp(const SolveTspSyncRequest& request) const;

        /**
         * Initiates TSP solution request and passes periodical updates (current progress, results, status) to consumer. Blocks until result is ready or timeout is exceeded
         */
        void solveTsp(const SolveTspStreamingRequest& request, const StreamingResponseConsumer& response_consumer) const;

    private:
        std::shared_ptr<grpc::ChannelInterface> channel;
        std::unique_ptr<assfire::api::v1::service::tsp::TspService::Stub> stub;
    };
}