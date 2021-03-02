#pragma once

#include <memory>
#include "assfire/router/client/RouterClient.hpp"
#include "assfire/api/v1/scheduler/service.grpc.pb.h"
#include "assfire/scheduler/engine/WaybillSchedulerFactory.hpp"

namespace assfire::scheduler {
    class SchedulerService : public assfire::api::v1::scheduler::SchedulerService::Service {
    public:
        using ScheduleWaybillRequest = ::assfire::api::v1::scheduler::ScheduleWaybillRequest;
        using ScheduleWaybillResponse = ::assfire::api::v1::scheduler::ScheduleWaybillResponse;
        using RouterClient = router::RouterClient;

        struct Options {
            std::string router_host = "http://localhost";
            int32_t router_port = 50051;
            bool use_ssl_for_router = false;
        };

        explicit SchedulerService(const Options &options);

        grpc::Status ScheduleWaybill(::grpc::ServerContext *context, const ScheduleWaybillRequest *request, ScheduleWaybillResponse *response) override;

    private:
        WaybillSchedulerFactory waybill_scheduler_factory;
        std::unique_ptr<RouterClient> router_client;
    };
}
