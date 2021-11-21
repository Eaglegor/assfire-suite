#include "SchedulerService.hpp"
#include "assfire/api/v1/scheduler/translators/Translators.hpp"
#include "assfire/api/v1/router/translators/Translators.hpp"

using namespace assfire::scheduler;

SchedulerService::SchedulerService(const SchedulerService::Options &options) {
    router_client = std::make_unique<RouterClient>(options.router_host, options.router_port, options.use_ssl_for_router);
    scheduler_engine = std::make_unique<SchedulerEngine>(
            WaybillSchedulingContext(
                    [&](const router::RouterEngineType engine_type, const router::RouteProviderSettings &settings, const router::RoutingProfile &routing_profile) {
                        return router_client->createDistanceMatrix(engine_type, router::DistanceMatrixCachingPolicy::NO_CACHING, routing_profile, settings);
                    })
    );
}

grpc::Status SchedulerService::ScheduleWaybill(::grpc::ServerContext *context, const SchedulerService::ScheduleWaybillRequest *request, SchedulerService::ScheduleWaybillResponse *response) {
    try {
        Waybill waybill = WaybillTranslator::fromProto(request->waybill());

        scheduler_engine->scheduleWaybillInPlace(waybill,
                                                WaybillSchedulingAlgorithmTypeTranslator::fromProto(request->algorithm_type()),
                                                WaybillSchedulerSettingsTranslator::fromProto(request->settings()),
                                                RoutingProfileTranslator::fromProto(request->routing_profile())
        );

        response->mutable_waybill()->CopyFrom(WaybillTranslator::toProto(waybill));
        response->mutable_status()->set_code(api::v1::scheduler::ScheduleWaybillResponseStatus::SCHEDULE_WAYBILL_RESPONSE_STATUS_CODE_OK);
        return grpc::Status::OK;
    } catch (const std::exception &e) {
        response->clear_waybill();
        response->mutable_status()->set_code(api::v1::scheduler::ScheduleWaybillResponseStatus::SCHEDULE_WAYBILL_RESPONSE_STATUS_CODE_ERROR);
        response->mutable_status()->set_message(e.what());
        return grpc::Status(grpc::StatusCode::UNKNOWN, e.what());
    }
}

