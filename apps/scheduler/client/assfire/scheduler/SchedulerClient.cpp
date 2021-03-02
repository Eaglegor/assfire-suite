#include "SchedulerClient.hpp"
#include "RemoteSchedulerGrpcConnector.hpp"

using namespace assfire::scheduler;
using namespace assfire::router;

SchedulerClient::SchedulerClient(std::unique_ptr<SchedulerGrpcConnector> grpc_connector) :
        grpc_connector(std::move(grpc_connector)) {
}

SchedulerClient::SchedulerClient(const std::string &host, int port, bool use_ssl) :
        grpc_connector(std::make_unique<RemoteSchedulerGrpcConnector>(host, port, use_ssl)) {
}

WaybillScheduler SchedulerClient::createWaybillScheduler(WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const RoutingProfile &routing_profile) const {
    return scheduler_factory.createWaybillScheduler(type, settings, routing_profile, *grpc_connector);
}

Waybill SchedulerClient::scheduleWaybill(const Waybill &waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const RoutingProfile &routing_profile) const {
    return createWaybillScheduler(type, settings, routing_profile).scheduleWaybill(waybill);
}

void SchedulerClient::scheduleWaybillInPlace(Waybill &waybill, WaybillSchedulingAlgorithmType type, const WaybillSchedulerSettings &settings, const RoutingProfile &routing_profile) const {
    createWaybillScheduler(type, settings, routing_profile).scheduleWaybillInPlace(waybill);
}
