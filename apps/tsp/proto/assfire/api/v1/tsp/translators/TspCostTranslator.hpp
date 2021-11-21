#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspCost.hpp"

namespace assfire::api::v1::tsp {
    class TspCostTranslator {
    public:
        using ProtoTspCost = assfire::api::v1::tsp::TspCost;
        using ApiTspCost = assfire::tsp::TspCost;

        static ApiTspCost fromProto(const ProtoTspCost &value);
        static ProtoTspCost toProto(const ApiTspCost &value);
    };
}
