#include "ClientWaybillSchedulerFactory.hpp"
#include "RemoteWaybillSchedulingAlgorithm.hpp"

using namespace assfire::scheduler;

ClientWaybillSchedulerFactory::ClientWaybillSchedulerFactory(std::unique_ptr<SchedulerGrpcConnector> grpc_connector)
        : grpc_connector(std::move(grpc_connector)) {}

WaybillScheduler
ClientWaybillSchedulerFactory::createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const router::RoutingProfile &routing_profile) const {
    return WaybillScheduler(std::make_shared<RemoteWaybillSchedulingAlgorithm>(*grpc_connector, type, settings, routing_profile));
}
