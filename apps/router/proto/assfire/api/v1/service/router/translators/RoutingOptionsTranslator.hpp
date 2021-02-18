#pragma once

#include <assfire/api/v1/service/router/router.grpc.pb.h>
#include <assfire/api/router/RouteProviderSettings.hpp>
#include <assfire/api/router/RouterEngineType.hpp>
#include "RoutingEngineTypeTranslator.hpp"

namespace assfire::router::proto_translation {

    class RoutingOptionsTranslator {
    public:
        static RouteProviderSettings fromProto(const assfire::api::v1::model::routing::RoutingOptions &options) {
            RouteProviderSettings result;
            if(options.has_osrm()) result.setOsrmSettings(fromProtoOsrmSettings(options.osrm()));
            result.setRetrieveWaypoints(options.retrieve_waypoints());
            result.setForceUpdate(options.force_update());
            return result;
        }

        static assfire::api::v1::model::routing::RoutingOptions toProto(const RouteProviderSettings &settings, RouterEngineType engine_type) {
            assfire::api::v1::model::routing::RoutingOptions result;
            result.set_routing_type(RoutingEngineTypeTranslator::toProto(engine_type));
            result.set_force_update(settings.isForceUpdate());
            result.set_retrieve_waypoints(settings.isRetrieveWaypoints());
            result.mutable_osrm()->CopyFrom(toProtoOsrmSettings(settings.getOsrmSettings()));
            return result;
        }

    private:
        static RouteProviderSettings::Osrm fromProtoOsrmSettings(const assfire::api::v1::model::routing::RoutingOptions::Osrm &options) {
            RouteProviderSettings::Osrm result;
            result.setGeometry(fromProtoOsrmGeometry(options.geometry()));
            return result;
        }

        static RouteProviderSettings::Osrm::Geometry fromProtoOsrmGeometry(const assfire::api::v1::model::routing::RoutingOptions::Osrm::Geometry &geometry) {
            switch (geometry) {
                case assfire::api::v1::model::routing::RoutingOptions::Osrm::GEOMETRY_SIMPLIFIED:
                    return RouteProviderSettings::Osrm::Geometry::SIMPLIFIED;
                case assfire::api::v1::model::routing::RoutingOptions::Osrm::GEOMETRY_FULL:
                    return RouteProviderSettings::Osrm::Geometry::FULL;
                case assfire::api::v1::model::routing::RoutingOptions::Osrm::GEOMETRY_STRAIGHT_LINE:
                    return RouteProviderSettings::Osrm::Geometry::STRAIGHT_LINE;
                default:
                    throw std::invalid_argument("Unknown or unsupported OSRM geometry type");
            }
        }

        static assfire::api::v1::model::routing::RoutingOptions::Osrm toProtoOsrmSettings(const RouteProviderSettings::Osrm &settings) {
            assfire::api::v1::model::routing::RoutingOptions::Osrm result;
            result.set_geometry(toProtoOsrmGeometry(settings.getGeometry()));
            return result;
        }

        static assfire::api::v1::model::routing::RoutingOptions::Osrm::Geometry toProtoOsrmGeometry(const RouteProviderSettings::Osrm::Geometry &geometry) {
            switch (geometry) {
                case RouteProviderSettings::Osrm::Geometry::SIMPLIFIED:
                    return assfire::api::v1::model::routing::RoutingOptions::Osrm::GEOMETRY_SIMPLIFIED;
                case RouteProviderSettings::Osrm::Geometry::FULL:
                    return assfire::api::v1::model::routing::RoutingOptions::Osrm::GEOMETRY_FULL;
                case RouteProviderSettings::Osrm::Geometry::STRAIGHT_LINE:
                    return assfire::api::v1::model::routing::RoutingOptions::Osrm::GEOMETRY_STRAIGHT_LINE;
                default:
                    throw std::invalid_argument("Unknown or unsupported OSRM geometry type");
            }
        }
    };

}

