#include "TransportSchedulerService.hpp"

using namespace assfire::scheduler::transport;

TransportSchedulerService::TransportSchedulerService(const TransportSchedulerService::Options &options)
{
    router_client = std::make_unique<RouterClient>(options.router_host, options.router_port, options.use_ssl_for_router);
    scheduler = std::make_unique<Scheduler>(*router_client);
}

grpc::Status TransportSchedulerService::BuildSchedule(::grpc::ServerContext *context, const BuildScheduleRequest *request, BuildScheduleResponse *response)
{
    try {
        response->mutable_schedule()->CopyFrom(scheduler->buildSchedule(request->task()));
        response->mutable_status()->set_code(BuildScheduleResponse::Status::STATUS_CODE_OK);
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        response->clear_schedule();
        response->mutable_status()->set_code(BuildScheduleResponse::Status::STATUS_CODE_ERROR);
        response->mutable_status()->set_message(e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

