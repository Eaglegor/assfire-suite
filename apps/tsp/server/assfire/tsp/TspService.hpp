#pragma once

#include <assfire/api/v1/service/tsp/tsp.grpc.pb.h>
#include <assfire/tsp/MetricsCollector.hpp>
#include <memory>
#include <atomic>

namespace assfire::router{
    class RouterClient;
}

namespace assfire::tsp
{
    class TspSolver;

    class TspService final : public assfire::api::v1::service::tsp::TspService::Service
    {
    public:
        using SolveTspSyncRequest = assfire::api::v1::service::tsp::SolveTspSyncRequest;
        using SolveTspSyncResponse = assfire::api::v1::service::tsp::SolveTspSyncResponse;
        using SolveTspStreamingRequest = assfire::api::v1::service::tsp::SolveTspStreamingRequest;
        using SolveTspStreamingResponse = assfire::api::v1::service::tsp::SolveTspStreamingResponse;
        using ResponseStatus = assfire::api::v1::service::tsp::ResponseStatus;

        struct Options
        {
            MetricsCollector metrics_collector;

            std::string router_host = "http://localhost";
            int32_t router_port = 50051;
            bool use_ssl_for_router = false;
        };

        TspService(const Options &);

        ~TspService();

        grpc::Status SolveTspSync(grpc::ServerContext *, const SolveTspSyncRequest *, SolveTspSyncResponse *) override;

        grpc::Status SolveTspStreaming(grpc::ServerContext *, const SolveTspStreamingRequest *, grpc::ServerWriter<SolveTspStreamingResponse> *) override;

    private:
        std::atomic_long request_id_counter;
        MetricsCollector metrics_collector;
        std::unique_ptr<router::RouterClient> router_client;
        std::unique_ptr<TspSolver> tsp_solver;
    };
}