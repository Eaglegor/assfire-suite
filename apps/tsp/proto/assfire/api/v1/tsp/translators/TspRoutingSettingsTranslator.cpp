#include "TspRoutingSettingsTranslator.hpp"
#include "assfire/api/v1/router/translators/RouterEngineTypeTranslator.hpp"
#include "assfire/api/v1/router/translators/RouteProviderSettingsTranslator.hpp"
#include "assfire/api/v1/router/translators/RoutingProfileTranslator.hpp"

namespace assfire::api::v1::tsp {
    using namespace router;

    TspRoutingSettingsTranslator::ApiRoutingSettings TspRoutingSettingsTranslator::fromProto(const TspRoutingSettingsTranslator::ProtoRoutingSettings &value) {
        return ApiRoutingSettings(
                RouterEngineTypeTranslator::fromProto(value.route_provider_settings().router_engine_type()),
                RouteProviderSettingsTranslator::fromProto(value.route_provider_settings()),
                RoutingProfileTranslator::fromProto(value.routing_profile())
        );
    }

    TspRoutingSettingsTranslator::ProtoRoutingSettings TspRoutingSettingsTranslator::toProto(const TspRoutingSettingsTranslator::ApiRoutingSettings &value) {
        ProtoRoutingSettings result;
        result.mutable_route_provider_settings()->CopyFrom(RouteProviderSettingsTranslator::toProto(value.getRouteProviderSettings(), value.getEngineType()));
        result.mutable_routing_profile()->CopyFrom(RoutingProfileTranslator::toProto(value.getRoutingProfile()));
        return result;
    }
}