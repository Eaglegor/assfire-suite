#pragma once

#include <grpcpp/channel.h>
#include "assfire/api/v1/scheduler/service.grpc.pb.h"
#include "SchedulerGrpcConnector.hpp"

namespace assfire::scheduler
{
    class RemoteSchedulerGrpcConnector : public SchedulerGrpcConnector
    {
    public:
        RemoteSchedulerGrpcConnector(const std::string &host, int port, bool use_ssl);

        ScheduleWaybillResponse scheduleWaybill(const ScheduleWaybillRequest &request) const override;

    private:
        std::shared_ptr<grpc::ChannelInterface> channel;
        std::unique_ptr<assfire::api::v1::scheduler::SchedulerService::Stub> stub;
    };
}