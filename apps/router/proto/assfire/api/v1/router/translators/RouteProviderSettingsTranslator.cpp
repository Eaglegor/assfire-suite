#include "RouteProviderSettingsTranslator.hpp"

using namespace assfire::api::v1::router;

RouteProviderSettingsTranslator::ApiRouteProviderSettings RouteProviderSettingsTranslator::fromProto(const ProtoRouteProviderSettings &proto)
{
    ApiRouteProviderSettings result;
    if(proto.has_osrm_settings()) result.setOsrmSettings(fromProtoOsrmSettings(proto.osrm_settings()));
    result.setRetrieveWaypoints(proto.retrieve_waypoints());
    result.setForceUpdate(proto.force_update());
    return result;
}

RouteProviderSettingsTranslator::ProtoRouteProviderSettings RouteProviderSettingsTranslator::toProto(const ApiRouteProviderSettings &settings, ApiRouterEngineType engine_type)
{
    ProtoRouteProviderSettings result;
    result.set_router_engine_type(RouterEngineTypeTranslator::toProto(engine_type));
    result.set_force_update(settings.isForceUpdate());
    result.set_retrieve_waypoints(settings.isRetrieveWaypoints());
    result.mutable_osrm_settings()->CopyFrom(toProtoOsrmSettings(settings.getOsrmSettings()));
    return result;
}

RouteProviderSettingsTranslator::ApiOsrmSettings RouteProviderSettingsTranslator::fromProtoOsrmSettings(const ProtoOsrmSettings &options)
{
    ApiOsrmSettings result;
    result.setGeometry(fromProtoOsrmGeometry(options.geometry()));
    return result;
}

RouteProviderSettingsTranslator::ApiOsrmGeometry RouteProviderSettingsTranslator::fromProtoOsrmGeometry(const ProtoOsrmGeometry &geometry)
{
    switch (geometry) {
        case ProtoOsrmGeometry::OSRM_GEOMETRY_SIMPLIFIED:
            return ApiOsrmGeometry::SIMPLIFIED;
        case ProtoOsrmGeometry::OSRM_GEOMETRY_FULL:
            return ApiOsrmGeometry::FULL;
        case ProtoOsrmGeometry::OSRM_GEOMETRY_STRAIGHT_LINE:
            return ApiOsrmGeometry::STRAIGHT_LINE;
        default:
            throw std::invalid_argument("Unknown or unsupported OSRM geometry type");
    }
}

RouteProviderSettingsTranslator::ProtoOsrmSettings RouteProviderSettingsTranslator::toProtoOsrmSettings(const ApiOsrmSettings &settings)
{
    ProtoOsrmSettings result;
    result.set_geometry(toProtoOsrmGeometry(settings.getGeometry()));
    return result;
}

RouteProviderSettingsTranslator::ProtoOsrmGeometry RouteProviderSettingsTranslator::toProtoOsrmGeometry(const ApiOsrmGeometry &geometry)
{
    switch (geometry) {
        case ApiOsrmGeometry::SIMPLIFIED:
            return ProtoOsrmGeometry::OSRM_GEOMETRY_SIMPLIFIED;
        case ApiOsrmGeometry::FULL:
            return ProtoOsrmGeometry::OSRM_GEOMETRY_FULL;
        case ApiOsrmGeometry::STRAIGHT_LINE:
            return ProtoOsrmGeometry::OSRM_GEOMETRY_STRAIGHT_LINE;
        default:
            throw std::invalid_argument("Unknown or unsupported OSRM geometry type");
    }
}
