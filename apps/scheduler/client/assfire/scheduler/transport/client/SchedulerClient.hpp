#pragma once

#include <assfire/api/v1/service/scheduler/transport/transport.grpc.pb.h>
#include <grpcpp/channel.h>
#include <functional>

namespace assfire::scheduler::transport
{
    class SchedulerClient
    {
    public:
        using BuildScheduleRequest = ::assfire::api::v1::service::scheduler::transport::BuildScheduleRequest;
        using BuildScheduleResponse = ::assfire::api::v1::service::scheduler::transport::BuildScheduleResponse;

        SchedulerClient(const std::string &host, int port, bool use_ssl);

        BuildScheduleResponse buildSchedule(const BuildScheduleRequest&) const;

    private:
        std::shared_ptr<grpc::ChannelInterface> channel;
        std::unique_ptr<assfire::api::v1::service::scheduler::transport::SchedulerService::Stub> stub;
    };
}