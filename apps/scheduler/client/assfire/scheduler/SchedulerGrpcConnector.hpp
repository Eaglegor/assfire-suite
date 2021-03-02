#pragma once

#include <assfire/api/v1/scheduler/service.grpc.pb.h>

namespace assfire::scheduler {
    class SchedulerGrpcConnector {
    public:
        using ScheduleWaybillRequest = assfire::api::v1::scheduler::ScheduleWaybillRequest;
        using ScheduleWaybillResponse = assfire::api::v1::scheduler::ScheduleWaybillResponse;
        using ResponseStatus = assfire::api::v1::scheduler::ScheduleWaybillResponseStatus;

        virtual ~SchedulerGrpcConnector() = default;

        virtual ScheduleWaybillResponse scheduleWaybill(const ScheduleWaybillRequest &) const = 0;
    };
}
