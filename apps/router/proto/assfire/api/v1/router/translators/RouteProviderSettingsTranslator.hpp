#pragma once

#include <assfire/api/v1/router/concepts.pb.h>
#include <assfire/router/api/RouteProviderSettings.hpp>
#include <assfire/router/api/RouterEngineType.hpp>
#include "RouterEngineTypeTranslator.hpp"

namespace assfire::api::v1::router {

    class RouteProviderSettingsTranslator {
    public:
        using ProtoRouteProviderSettings = assfire::api::v1::router::RouteProviderSettings;
        using ProtoOsrmGeometry = assfire::api::v1::router::OsrmGeometry;
        using ProtoOsrmSettings = assfire::api::v1::router::OsrmSettings;

        using ApiRouteProviderSettings = assfire::router::RouteProviderSettings;
        using ApiOsrmGeometry = assfire::router::OsrmGeometry;
        using ApiOsrmSettings = assfire::router::OsrmSettings;
        using ApiRouterEngineType = assfire::router::RouterEngineType;

        static ApiRouteProviderSettings fromProto(const ProtoRouteProviderSettings &proto);

        static ProtoRouteProviderSettings toProto(const ApiRouteProviderSettings &settings, ApiRouterEngineType engine_type);

    private:
        static ApiOsrmSettings fromProtoOsrmSettings(const ProtoOsrmSettings &options);

        static ApiOsrmGeometry fromProtoOsrmGeometry(const ProtoOsrmGeometry &geometry);

        static ProtoOsrmSettings toProtoOsrmSettings(const ApiOsrmSettings &settings);

        static ProtoOsrmGeometry toProtoOsrmGeometry(const ApiOsrmGeometry &geometry);
    };

}

