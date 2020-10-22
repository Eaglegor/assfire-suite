#include "TspService.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <assfire/router/client/RouterClient.hpp>
#include <assfire/scheduler/transport/Scheduler.hpp>
#include <assfire/estimator/transport/ScheduleEstimator.hpp>
#include <assfire/tsp/TspSolver.hpp>

using namespace assfire::tsp;
using namespace assfire::router;
using namespace assfire::scheduler::transport;
using namespace assfire::estimator::transport;

TspService::TspService(const Options &options)
        : request_id_counter(0)
{
    metrics_collector = options.metrics_collector;

    router_client = std::make_unique<RouterClient>(options.router_host, options.router_port, options.use_ssl_for_router);
    scheduler = std::make_unique<Scheduler>(*router_client);
    estimator = std::make_unique<ScheduleEstimator>();

    tsp_solver = std::make_unique<TspSolver>(*router_client, *scheduler, *estimator);
}

grpc::Status TspService::SolveTspSync(grpc::ServerContext *context, const TspService::SolveTspSyncRequest *request, TspService::SolveTspSyncResponse *response)
{
    SPDLOG_INFO("Processing sync solveTsp request");
    try {
        response->mutable_solution()->CopyFrom(tsp_solver->solveTsp(request->task(), request->settings()));
        response->mutable_status()->set_code(ResponseStatus::RESPONSE_STATUS_CODE_OK);
        response->mutable_status()->set_progress(100);
        return grpc::Status::OK;
    } catch (std::exception &e) {
        SPDLOG_ERROR("Error while processing sync solveTsp request: {}", e.what());
        response->mutable_status()->set_message(e.what());
        response->mutable_status()->set_code(ResponseStatus::RESPONSE_STATUS_CODE_ERROR);
        response->clear_solution();
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

grpc::Status TspService::SolveTspStreaming(grpc::ServerContext *context, const TspService::SolveTspStreamingRequest *request, grpc::ServerWriter<TspService::SolveTspStreamingResponse> *stream)
{
    SPDLOG_INFO("Starting streaming solveTsp request");
    SolveTspStreamingResponse response;
    try {
        response.mutable_solution()->CopyFrom(tsp_solver->solveTsp(request->task(), request->settings(), [&](const ResponseStatus& status){
            response.mutable_status()->CopyFrom(status);
            stream->Write(response);
        }));
        response.mutable_status()->set_code(ResponseStatus::RESPONSE_STATUS_CODE_OK);
        response.mutable_status()->set_progress(100);
        stream->Write(response);
        return grpc::Status::OK;
    } catch (std::exception &e) {
        response.mutable_status()->set_message(e.what());
        response.mutable_status()->set_code(ResponseStatus::RESPONSE_STATUS_CODE_ERROR);
        response.clear_solution();
        stream->Write(response);
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

TspService::~TspService() = default;
