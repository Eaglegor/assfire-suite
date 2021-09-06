#include "ClientWaybillSchedulerFactory.hpp"
#include "RemoteWaybillSchedulingAlgorithm.hpp"

using namespace assfire::scheduler;

WaybillScheduler ClientWaybillSchedulerFactory::createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings& settings, const router::RoutingProfile& routing_profile, const SchedulerGrpcConnector& grpc_connector) const {
    return WaybillScheduler(std::make_shared<RemoteWaybillSchedulingAlgorithm>(grpc_connector, type, settings, routing_profile));
}
