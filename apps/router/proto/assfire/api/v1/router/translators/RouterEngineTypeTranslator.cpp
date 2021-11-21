#include "RouterEngineTypeTranslator.hpp"

using namespace assfire::api::v1::router;

RouterEngineTypeTranslator::ApiRouterEngineType RouterEngineTypeTranslator::fromProto(const ProtoRouterEngineType &type)
{
    switch (type) {
        case ProtoRouterEngineType::ROUTER_ENGINE_TYPE_CROWFLIGHT:
            return ApiRouterEngineType::CROWFLIGHT;
        case ProtoRouterEngineType::ROUTER_ENGINE_TYPE_EUCLIDEAN:
            return ApiRouterEngineType::EUCLIDEAN;
        case ProtoRouterEngineType::ROUTER_ENGINE_TYPE_OSRM:
            return ApiRouterEngineType::OSRM;
        case ProtoRouterEngineType::ROUTER_ENGINE_TYPE_RANDOM:
            return ApiRouterEngineType::RANDOM;
        default:
            throw std::invalid_argument("Unknown or unsupported routing engine type");
    }
}

RouterEngineTypeTranslator::ProtoRouterEngineType RouterEngineTypeTranslator::toProto(const ApiRouterEngineType &type)
{
    switch (type) {
        case ApiRouterEngineType::CROWFLIGHT:
            return ProtoRouterEngineType::ROUTER_ENGINE_TYPE_CROWFLIGHT;
        case ApiRouterEngineType::EUCLIDEAN:
            return ProtoRouterEngineType::ROUTER_ENGINE_TYPE_EUCLIDEAN;
        case ApiRouterEngineType::OSRM:
            return ProtoRouterEngineType::ROUTER_ENGINE_TYPE_OSRM;
        case ApiRouterEngineType::RANDOM:
            return ProtoRouterEngineType::ROUTER_ENGINE_TYPE_RANDOM;
        default:
            throw std::invalid_argument("Unknown or unsupported routing engine type");
    }
}
