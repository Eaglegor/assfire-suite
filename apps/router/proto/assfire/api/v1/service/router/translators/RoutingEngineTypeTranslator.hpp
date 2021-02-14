#pragma once

#include <assfire/api/v1/service/router/router.grpc.pb.h>
#include <assfire/api/router/RouterEngineType.hpp>

namespace assfire::router::proto_translation {

    struct RoutingEngineTypeTranslator {
        static RouterEngineType fromProto(const assfire::api::v1::model::routing::RoutingOptions::RoutingType& type) {
            switch (type) {
                case assfire::api::v1::model::routing::RoutingOptions::ROUTING_TYPE_CROWFLIGHT:
                    return RouterEngineType::CROWFLIGHT;
                case assfire::api::v1::model::routing::RoutingOptions::ROUTING_TYPE_EUCLIDEAN:
                    return RouterEngineType::EUCLIDEAN;
                case assfire::api::v1::model::routing::RoutingOptions::ROUTING_TYPE_OSRM:
                    return RouterEngineType::OSRM;
                case assfire::api::v1::model::routing::RoutingOptions::ROUTING_TYPE_RANDOM:
                    return RouterEngineType::RANDOM;
                default:
                    throw std::invalid_argument("Unknown or unsupported routing engine type");
            }
        }

        static assfire::api::v1::model::routing::RoutingOptions::RoutingType toProto(const RouterEngineType& type) {
            switch (type) {
                case RouterEngineType::CROWFLIGHT:
                    return assfire::api::v1::model::routing::RoutingOptions::ROUTING_TYPE_CROWFLIGHT;
                case RouterEngineType::EUCLIDEAN:
                    return assfire::api::v1::model::routing::RoutingOptions::ROUTING_TYPE_EUCLIDEAN;
                case RouterEngineType::OSRM:
                    return assfire::api::v1::model::routing::RoutingOptions::ROUTING_TYPE_OSRM;
                case RouterEngineType::RANDOM:
                    return assfire::api::v1::model::routing::RoutingOptions::ROUTING_TYPE_RANDOM;
                default:
                    throw std::invalid_argument("Unknown or unsupported routing engine type");
            }
        }
    };

}

