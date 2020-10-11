#pragma once

#include <memory>
#include <assfire/router/client/RouterClient.hpp>
#include <assfire/api/v1/service/scheduler/transport/transport.grpc.pb.h>
#include <assfire/scheduler/MetricsCollector.hpp>
#include <assfire/scheduler/transport/Scheduler.hpp>

namespace assfire::scheduler::transport
{
    class TransportSchedulerService : public api::v1::service::scheduler::transport::SchedulerService::Service
    {
    public:
        using BuildScheduleRequest = ::assfire::api::v1::service::scheduler::transport::BuildScheduleRequest;
        using BuildScheduleResponse = ::assfire::api::v1::service::scheduler::transport::BuildScheduleResponse;
        using SchedulerResult = Scheduler::SchedulerResult;
        using SchedulerTask = Scheduler::SchedulerTask;
        using RouterClient = router::RouterClient;

        struct Options
        {
            MetricsCollector metrics_collector;

            std::string router_host = "http://localhost";
            int32_t router_port = 50051;
            bool use_ssl_for_router = false;
        };

        explicit TransportSchedulerService(const Options &options);

        grpc::Status BuildSchedule(::grpc::ServerContext *context, const BuildScheduleRequest *request, BuildScheduleResponse *response) override;

    private:
        std::unique_ptr<RouterClient> router_client;
        std::unique_ptr<Scheduler> scheduler;
    };
}
