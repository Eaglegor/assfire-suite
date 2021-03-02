#include "RemoteWaybillSchedulingAlgorithm.hpp"
#include "assfire/api/v1/scheduler/translators/Translators.hpp"
#include "assfire/api/v1/router/translators/Translators.hpp"

using namespace assfire::scheduler;
using namespace assfire::api::v1::scheduler;

RemoteWaybillSchedulingAlgorithm::RemoteWaybillSchedulingAlgorithm(const SchedulerGrpcConnector &connector, WaybillSchedulingAlgorithmType algorithm_type, const WaybillSchedulerSettings& settings,
                                                                   const router::RoutingProfile& routing_profile) :
        connector(connector), algorithm_type(algorithm_type), settings(settings), routing_profile(routing_profile) {
}

void RemoteWaybillSchedulingAlgorithm::scheduleWaybill(Waybill &waybill) const {
    ScheduleWaybillRequest request;

    request.set_algorithm_type(WaybillSchedulingAlgorithmTypeTranslator::toProto(algorithm_type));
    request.mutable_waybill()->CopyFrom(WaybillTranslator::toProto(waybill));
    request.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(routing_profile));
    request.mutable_settings()->CopyFrom(WaybillSchedulerSettingsTranslator::toProto(settings));

    ScheduleWaybillResponse response = connector.scheduleWaybill(request);

    if(response.status().code() != ScheduleWaybillResponseStatus::SCHEDULE_WAYBILL_RESPONSE_STATUS_CODE_OK) {
        throw std::runtime_error(response.status().message());
    }

    waybill = WaybillTranslator::fromProto(response.waybill());
}
