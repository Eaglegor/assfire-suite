#include "TspService.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <assfire/router/client/RouterClient.hpp>
#include <assfire/tsp/TspSolver.hpp>

using namespace assfire::tsp;
using namespace assfire::router;

TspService::TspService(const Options &options)
        : request_id_counter(0)
{
    metrics_collector = options.metrics_collector;

    router_client = std::make_unique<RouterClient>(options.router_host, options.router_port, options.use_ssl_for_router);
    tsp_solver = std::make_unique<TspSolver>(*router_client);
}

grpc::Status TspService::SolveTspSync(grpc::ServerContext *, const TspService::SolveTspSyncRequest *, TspService::SolveTspSyncResponse *)
{
    return grpc::Status::OK;
}

grpc::Status TspService::SolveTspStreaming(grpc::ServerContext *, const TspService::SolveTspStreamingRequest *, grpc::ServerWriter<TspService::SolveTspStreamingResponse> *)
{
    return grpc::Status::OK;
}

TspService::~TspService() = default;
