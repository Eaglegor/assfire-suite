#pragma once

#include <assfire/api/v1/router/concepts.pb.h>
#include <assfire/router/api/RouterEngineType.hpp>

namespace assfire::api::v1::router {

    class RouterEngineTypeTranslator {
    public:

        using ProtoRouterEngineType = assfire::api::v1::router::RouterEngineType;
        using ApiRouterEngineType = assfire::router::RouterEngineType;

        static ApiRouterEngineType fromProto(const ProtoRouterEngineType& type);

        static ProtoRouterEngineType toProto(const ApiRouterEngineType& type);
    };

}

