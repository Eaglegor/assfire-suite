#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspPoint.hpp"

namespace assfire::api::v1::tsp {
    class TspPointTranslator {
    public:
        using ProtoTspPoint = assfire::api::v1::tsp::TspPoint;

        using ApiTspPoint = assfire::tsp::TspPoint;

        static ApiTspPoint fromProto(const ProtoTspPoint &value);
        static ProtoTspPoint toProto(const ApiTspPoint &value);
    };
}

