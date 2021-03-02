#include "WaybillSchedulerSettingsTranslator.hpp"
#include "assfire/api/v1/router/translators/Translators.hpp"

using namespace assfire::api::v1::scheduler;

WaybillSchedulerSettingsTranslator::ApiWaybillSchedulerSettings WaybillSchedulerSettingsTranslator::fromProto(const WaybillSchedulerSettingsTranslator::ProtoWaybillSchedulerSettings &obj) {
    return ApiWaybillSchedulerSettings(
            RouteProviderSettingsTranslator::fromProto(obj.route_provider_settings()),
            RouterEngineTypeTranslator::fromProto(obj.route_provider_settings().router_engine_type())
    );
}

WaybillSchedulerSettingsTranslator::ProtoWaybillSchedulerSettings WaybillSchedulerSettingsTranslator::toProto(const WaybillSchedulerSettingsTranslator::ApiWaybillSchedulerSettings &obj) {
    ProtoWaybillSchedulerSettings result;
    result.mutable_route_provider_settings()->CopyFrom(RouteProviderSettingsTranslator::toProto(obj.getRouteProviderSettings(), obj.getRouterEngineType()));
    return result;
}
