#pragma once

#include "assfire/scheduler/api/WaybillSchedulingAlgorithmType.hpp"
#include "assfire/scheduler/api/WaybillSchedulingAlgorithm.hpp"
#include "assfire/scheduler/api/WaybillSchedulerSettings.hpp"
#include "assfire/router/api/RoutingProfile.hpp"
#include "SchedulerGrpcConnector.hpp"

namespace assfire::scheduler {
    class RemoteWaybillSchedulingAlgorithm : public WaybillSchedulingAlgorithm {
    public:
        RemoteWaybillSchedulingAlgorithm(const SchedulerGrpcConnector &connector, WaybillSchedulingAlgorithmType algorithm_type, const WaybillSchedulerSettings& settings,
                                         const router::RoutingProfile& routing_profile);

        void scheduleWaybill(Waybill &waybill) const override;

    private:
        const SchedulerGrpcConnector &connector;
        WaybillSchedulingAlgorithmType algorithm_type;
        WaybillSchedulerSettings settings;
        router::RoutingProfile routing_profile;
    };
}